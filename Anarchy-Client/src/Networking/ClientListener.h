#pragma once
#include "ClientSocket.h"
#include "Lib/SequenceBuffer.h"
#include "Lib/SocketApi.h"
#include "Lib/Entities/ActionBuffer.h"
#include "Core/Tasks/TaskManager.h"
#include "Core/Time/TimeDelta.h"

namespace Anarchy
{

	struct ServerDisconnect
	{
	};

	class ClientListener : public ClientSocketApi
	{
	public:
		inline static const double IgnoreTimeout = -1.0;

	private:
		EventBus m_Bus;
		TaskManager m_TaskManager;
		EventEmitter<ServerDisconnect> m_OnDisconnect;
		connid_t m_ConnectionId;
		bool m_Connecting;

		ClientSocket& m_Socket;

		seqid_t m_SequenceId;
		seqid_t m_RemoteSequenceId;

		ScopedEventListener m_Listener;
		std::unordered_map<MessageType, std::function<void(InputMemoryStream&)>> m_MessageHandlers;

		double m_TimeSinceLastSentMessage;
		double m_TimeSinceLastReceivedMessage;

		SequenceBuffer m_ReceivedMessages;
		SequenceBuffer m_SentMessages;

		ActionBuffer m_Actions;

	public:
		ClientListener(ClientSocket& socket);
		~ClientListener();

		seqid_t GetSequenceId() const;
		seqid_t GetRemoteSequenceId() const;
		EventEmitter<ServerDisconnect>& OnDisconnect();
		const ClientSocket& GetClientSocket() const;
		ClientSocket& GetClientSocket();
		bool IsConnecting() const;
		bool IsConnected() const;

		connid_t GetConnectionId() const;
		ActionBuffer& GetActionBuffer();

		void Update(TimeDelta delta);

		void SendKeepAlive() override;
		void OnKeepAlive(const NetworkMessage<KeepAlivePacket>& message) override;
		void OnForceDisconnect(const NetworkMessage<ForceDisconnectMessage>& message);

		Promise<ServerConnectionResponse> Connect(const ServerConnectionRequest& request, double timeoutSeconds = IgnoreTimeout) override;
		Promise<ServerDisconnectResponse> Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds = IgnoreTimeout) override;

		Promise<CreateCharacterResponse> CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds = IgnoreTimeout) override;
		Promise<GetEntitiesResponse> GetEntities(const GetEntitiesRequest& request, double timeoutSeconds = IgnoreTimeout) override;

		void SpawnEntities(const NetworkMessage<SpawnEntitiesRequest>& request) override;
		void DestroyEntities(const NetworkMessage<DestroyEntitiesRequest>& request) override;
		void UpdateEntities(const NetworkMessage<UpdateEntitiesRequest>& request) override;

		void SendAction(const GenericAction& command) override;

		template<typename TRequest>
		void Register(const std::function<void(const NetworkMessage<TRequest>&)>& callback)
		{
			MessageType messageType = TRequest::Type;
			BLT_ASSERT(m_MessageHandlers.find(messageType) == m_MessageHandlers.end(), "Handler already exists for message type");
			m_MessageHandlers[messageType] = [this, callback](InputMemoryStream& stream)
			{
				m_TaskManager.RunOnMainThread(make_shared_function([callback, stream{ std::move(stream) }]() mutable
				{
					NetworkMessage<TRequest> request;
					Deserialize(stream, request);
					callback(request);
				}));
			};
		}

	private:
		void DisconnectInternal();

		void IncrementSequenceId(seqid_t amount = 1);
		void SetRemoteSequenceId(seqid_t seqId);
		void ResetTimeSinceLastReceivedMessage();
		void ResetTimeSinceLastSentMessage();

		inline typename PacketData::timestamp_t GetTimestamp() const { return std::chrono::high_resolution_clock::now(); }

		template<typename T>
		ServerNetworkMessage<T> CreateMessage(const T& data)
		{
			ServerNetworkMessage<T> message;
			message.Header.SequenceId = GetSequenceId();
			message.Header.Ack = 0;
			message.Header.AckBitset = 0;
			message.ConnectionId = GetConnectionId();
			message.Message = data;
			return message;
		}

		template<typename T>
		std::optional<T> MakeOptional(const std::optional<NetworkMessage<T>>& message)
		{
			if (message)
			{
				return std::optional<T>(message->Message);
			}
			return std::optional<T>();
		}

		template<typename T>
		void HandleOutgoingMessage(ServerNetworkMessage<T>& message, bool requiresAck = true)
		{
			message.Header.AckBitset = 0;
			if (requiresAck)
			{
				PacketData& data = m_SentMessages.InsertPacketData(message.Header.SequenceId);
				data.Timestamp = GetTimestamp();
			}
			seqid_t base = GetRemoteSequenceId();
			PacketData* packet = m_ReceivedMessages.GetPacketData(base);
			if (packet != nullptr)
			{
				message.Header.Ack = base;
				packet->Acked = true;
				for (seqid_t i = 0; i < 32; i++)
				{
					// Find to underflow
					seqid_t seqid = base - (i + 1);
					PacketData* data = m_ReceivedMessages.GetPacketData(seqid);
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

		template<typename T>
		void HandleIncomingMessage(const NetworkMessage<T>& message, bool requiresAck = true)
		{
			auto timestamp = GetTimestamp();
			if (requiresAck)
			{
				PacketData& data = m_ReceivedMessages.InsertPacketData(message.Header.SequenceId);
				data.Timestamp = timestamp;
			}

			PacketData* packet = m_SentMessages.GetPacketData(message.Header.Ack);
			if (packet != nullptr)
			{
				if (!packet->Acked)
					HandlePacketAcked(message.Header.Ack, packet);

				for (seqid_t i = 0; i < 32; i++)
				{
					if (BLT_IS_BIT_SET(message.Header.AckBitset, i))
					{
						// Fine for seqid to underflow
						seqid_t seqid = message.Header.Ack - (i + 1);
						PacketData* data = m_SentMessages.GetPacketData(seqid);
						if (data && !data->Acked)
							HandlePacketAcked(seqid, data);
					}
				}
			}
		}

		void HandlePacketAcked(seqid_t sequenceId, PacketData* data) const;

		template<typename TResponse, typename TRequest>
		std::optional<NetworkMessage<TResponse>> AwaitResponse(const ServerNetworkMessage<TRequest>& request, double timeoutSeconds)
		{
			std::promise<std::optional<NetworkMessage<TResponse>>> promise;
			ScopedEventListener listener = GetClientSocket().OnMessageReceived().AddScopedEventListener([&promise](Event<ServerMessageReceived>& e)
				{
					if (e.Data.Type == TResponse::Type)
					{
						std::optional<NetworkMessage<TResponse>> response;
						Deserialize(e.Data.Data, response);
						if (response.has_value())
						{
							promise.set_value(std::move(response));
						}
						else
						{
							promise.set_value({});
						}
					}
				});
			std::future<std::optional<NetworkMessage<TResponse>>> future = promise.get_future();
			GetClientSocket().SendPacket(TRequest::Type, request);
			if (timeoutSeconds == IgnoreTimeout)
			{
				std::optional<NetworkMessage<TResponse>> value = future.get();
				if (value)
				{
					HandleIncomingMessage(value.value());
				}
				return value;
			}
			BLT_ASSERT(timeoutSeconds > 0.0, "Invalid timeout");
			std::future_status status = future.wait_for(std::chrono::nanoseconds((size_t)(timeoutSeconds * 1e9)));
			if (status == std::future_status::ready)
			{
				std::optional<NetworkMessage<TResponse>> value = future.get();
				if (value)
				{
					HandleIncomingMessage(value.value());
				}
				return value;
			}
			return {};
		}

	};

}