#pragma once
#include "ServerSocket.h"
#include "Lib/SocketApi.h"
#include "Lib/SequenceBuffer.h"
#include "Connections/ClientConnection.h"
#include "Core/Time/Time.h"

namespace Anarchy
{

	class ServerListener : public ServerSocketApi
	{
	private:
		ScopedEventListener m_Listener;
		ServerSocket& m_ServerSocket;
		std::unordered_map<MessageType, std::function<void(const SocketAddress&, InputMemoryStream&)>> m_MessageHandlers;

		std::mutex m_Mutex;

	public:
		ServerListener(ServerSocket& socket);

		template<typename TResponse, typename TRequest>
		void Register(const std::function<std::optional<TResponse>(const ServerRequest<ServerNetworkMessage<TRequest>>&)>& callback)
		{
			MessageType messageType = TRequest::Type;
			BLT_ASSERT(m_MessageHandlers.find(messageType) == m_MessageHandlers.end(), "Handler already exists for message type");
			m_MessageHandlers[messageType] = [callback, this](const SocketAddress& address, InputMemoryStream& data)
			{
				ServerRequest<ServerNetworkMessage<TRequest>> srequest;
				srequest.From = address;
				Deserialize(data, srequest.Request);
				std::optional<TResponse> response = callback(srequest);
				
				auto message = CreateOptionalMessage(srequest.Request.ConnectionId, response);
				if (message)
				{
					HandleOutgoingMessage(GetConnection(srequest.Request.ConnectionId), message.value());
				}
				m_ServerSocket.SendPacket(address, TResponse::Type, message);
			};
		}

		template<typename TRequest>
		void Register(const std::function<void(const ServerRequest<ServerNetworkMessage<TRequest>>&)>& callback)
		{
			MessageType messageType = TRequest::Type;
			BLT_ASSERT(m_MessageHandlers.find(messageType) == m_MessageHandlers.end(), "Handler already exists for message type");
			m_MessageHandlers[messageType] = [callback](const SocketAddress& address, InputMemoryStream& data)
			{
				ServerRequest<ServerNetworkMessage<TRequest>> srequest;
				srequest.From = address;
				Deserialize(data, srequest.Request);
				callback(srequest);
			};
		}

		void Update(TimeDelta delta);

		void OnKeepAlive(const ServerRequest<ServerNetworkMessage<KeepAlivePacket>>& packet) override;
		void SendKeepAlive(const std::vector<connid_t>& connections) override;
		void ForceDisconnectConnections(const std::vector<connid_t>& connectionIds) override;

		std::optional<ServerConnectionResponse> Connect(const ServerRequest<ServerNetworkMessage<ServerConnectionRequest>>& request) override;
		std::optional<ServerDisconnectResponse> Disconnect(const ServerRequest<ServerNetworkMessage<ServerDisconnectRequest>>& request) override;

		std::optional<CreateCharacterResponse> CreateCharacter(const ServerRequest<ServerNetworkMessage<CreateCharacterRequest>>& request) override;
		std::optional<GetEntitiesResponse> GetEntities(const ServerRequest<ServerNetworkMessage<GetEntitiesRequest>>& request) override;

		void SpawnEntities(const std::vector<connid_t>& connections, const SpawnEntitiesRequest& request, connid_t ownerConnectionId = InvalidConnectionId) override;
		void DestroyEntities(const std::vector<connid_t>& connections, const DestroyEntitiesRequest& request) override;
		void UpdateEntities(const std::vector<connid_t>& connections, const UpdateEntitiesRequest& request) override;

		void OnMoveCommand(const ServerNetworkMessage<EntityCommand<TileMovement>>& command) override;

	private:
		void SendKeepAliveInternal(const std::vector<connid_t>& connections);
		void ForceDisconnectConnectionsInternal(const std::vector<connid_t>& connectionIds);
		void SpawnEntitiesInternal(const std::vector<connid_t>& connections, const SpawnEntitiesRequest& request, connid_t ownerConnectionId = InvalidConnectionId);
		void DestroyEntitiesInternal(const std::vector<connid_t>& connections, const DestroyEntitiesRequest& request);
		void UpdateEntitiesInternal(const std::vector<connid_t>& connections, const UpdateEntitiesRequest& request);
		void CleanupConnection(connid_t connectionId);

	private:
		template<typename T>
		std::optional<NetworkMessage<T>> CreateOptionalMessage(connid_t connectionId, const std::optional<T>& data)
		{
			if (data.has_value())
			{
				NetworkMessage<T> message;
				message.Header.SequenceId = GetSequenceId(connectionId);
				message.Header.Ack = 0;
				message.Header.AckBitset = 0;
				message.Message = data.value();
				return message;
			}
			return {};
		}

		template<typename T>
		NetworkMessage<T> CreateMessage(connid_t connectionId, const T& data)
		{
			return CreateOptionalMessage<T>(connectionId, { data }).value();
		}

		template<typename T>
		void HandleOutgoingMessage(ClientConnection* connection, NetworkMessage<T>& message, bool requiresAck = true)
		{
			if (connection != nullptr)
			{
				message.Header.AckBitset = 0;
				if (requiresAck)
				{
					PacketData& data = connection->GetSentBuffer().InsertPacketData(message.Header.SequenceId);
					data.Timestamp = GetTimestamp();
				}
				SequenceBuffer& receivedBuffer = connection->GetReceivedBuffer();
				seqid_t base = connection->GetRemoteSequenceId();
				PacketData* packet = receivedBuffer.GetPacketData(base);
				if (packet != nullptr)
				{
					message.Header.Ack = base;
					packet->Acked = true;
					for (seqid_t i = 0; i < 32; i++)
					{
						// Find to underflow
						seqid_t seqid = base - (i + 1);
						PacketData* data = receivedBuffer.GetPacketData(seqid);
						if (data != nullptr)
						{
							message.Header.AckBitset |= BLT_BIT(i);
							data->Acked = true;
						}
					}
				}
				else
				{
					message.Header.Ack = base + (std::numeric_limits<seqid_t>::max() / 2);
				}
			}
		}

		template<typename T>
		void HandleIncomingMessage(const ServerNetworkMessage<T>& message, bool requiresAck = true)
		{
			ClientConnection* connection = GetConnection(message.ConnectionId);
			if (connection != nullptr)
			{
				auto timestamp = GetTimestamp();
				if (requiresAck)
				{
					PacketData& data = connection->GetReceivedBuffer().InsertPacketData(message.Header.SequenceId);
					data.Timestamp = timestamp;
				}

				PacketData* packet = connection->GetSentBuffer().GetPacketData(message.Header.Ack);
				if (packet != nullptr)
				{
					if (!packet->Acked)
						HandlePacketAcked(message.Header.Ack, packet, connection);

					for (seqid_t i = 0; i < 32; i++)
					{
						if (BLT_IS_BIT_SET(message.Header.AckBitset, i))
						{
							// Fine for seqid to underflow
							seqid_t seqid = message.Header.Ack - (i + 1);
							PacketData* data = connection->GetSentBuffer().GetPacketData(seqid);
							if (data && !data->Acked)
								HandlePacketAcked(seqid, data, connection);
						}
					}
				}
			}
		}

		void HandlePacketAcked(seqid_t sequenceId, PacketData* data, ClientConnection* connection) const;

		inline typename PacketData::timestamp_t GetTimestamp() const { return std::chrono::high_resolution_clock::now(); }

		ClientConnection* GetConnection(connid_t connectionId) const;
		seqid_t GetSequenceId(connid_t connectionId) const;
		ServerSocket& GetSocket() const;

	};

}