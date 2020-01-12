#pragma once
#include "ServerSocket.h"
#include "Lib/SocketApi.h"
#include "Connections/ClientConnection.h"

namespace Anarchy
{

	class ServerListener : public ServerSocketApi
	{
	private:
		ScopedEventListener m_Listener;
		ServerSocket& m_ServerSocket;

		std::unordered_map<MessageType, std::function<void(const SocketAddress&, InputMemoryStream&)>> m_MessageHandlers;

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
				m_ServerSocket.SendPacket(address, TResponse::Type, CreateOptionalMessage(srequest.Request.ConnectionId, response));
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

		std::optional<ServerConnectionResponse> Connect(const ServerRequest<ServerNetworkMessage<ServerConnectionRequest>>& request) override;
		std::optional<ServerDisconnectResponse> Disconnect(const ServerRequest<ServerNetworkMessage<ServerDisconnectRequest>>& request) override;

		std::optional<CreateCharacterResponse> CreateCharacter(const ServerRequest<ServerNetworkMessage<CreateCharacterRequest>>& request) override;
		std::optional<GetEntitiesResponse> GetEntities(const ServerRequest<ServerNetworkMessage<GetEntitiesRequest>>& request) override;

		void SpawnEntities(const std::vector<connid_t>& connections, const SpawnEntitiesRequest& request, connid_t ownerConnectionId = InvalidConnectionId) override;
		void DestroyEntities(const std::vector<connid_t>& connections, const DestroyEntitiesRequest& request) override;
		void UpdateEntities(const std::vector<connid_t>& connections, const UpdateEntitiesRequest& request) override;

		void OnMoveCommand(const EntityMoveCommand& command) override;

	private:
		template<typename T>
		std::optional<NetworkMessage<T>> CreateOptionalMessage(connid_t connectionId, const std::optional<T>& data)
		{
			if (data.has_value())
			{
				NetworkMessage<T> message;
				message.SequenceId = GetSequenceId(connectionId);
				message.Message = data.value();
				return message;
			}
			return {};
		}

		template<typename T>
		NetworkMessage<T> CreateMessage(connid_t connectionId, const T& data)
		{
			NetworkMessage<T> message;
			message.SequenceId = GetSequenceId(connectionId);
			message.Message = data;
			return message;
		}

		ClientConnection* GetConnection(connid_t connectionId) const;
		seqid_t GetSequenceId(connid_t connectionId) const;
		ServerSocket& GetSocket() const;

	};

}