#pragma once
#include "ServerConnection.h"
#include "Lib/SocketApi.h"

namespace Anarchy
{

	struct ServerDisconnect
	{

	};

	class ConnectionManager : public ClientSocketApi
	{
	public:
		inline static const double IgnoreTimeout = -1.0;

	private:
		EventBus m_Bus;
		TaskManager m_TaskManager;
		EventEmitter<ServerDisconnect> m_OnDisconnect;

		ServerConnection m_Connection;
		connid_t m_ConnectionId;
		bool m_Connecting;

		seqid_t m_SequenceId;
		seqid_t m_RemoteSequenceId;

		ScopedEventListener m_Listener;
		std::unordered_map<MessageType, std::function<void(InputMemoryStream&)>> m_MessageHandlers;

		double m_TimeSinceKeepAlive;
		double m_TimeSinceLastReceivedMessage;

	public:
		ConnectionManager(const SocketAddress& address);
		~ConnectionManager();

		bool IsConnecting() const;
		bool IsConnected() const;
		const ServerConnection& GetServerSocket() const;
		ServerConnection& GetServerSocket();

		connid_t GetConnectionId() const;
		seqid_t GetSequenceId() const;
		seqid_t GetRemoteSequenceId() const;
		EventEmitter<ServerDisconnect>& OnDisconnect();

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

		void SendMoveCommand(const EntityCommand<TileMovement>& command) override;

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

		template<typename T>
		ServerNetworkMessage<T> CreateMessage(const T& data)
		{
			ServerNetworkMessage<T> message;
			message.ConnectionId = GetConnectionId();
			message.Message = data;
			message.SequenceId = GetSequenceId();
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

		template<typename TResponse, typename TRequest>
		std::optional<NetworkMessage<TResponse>> AwaitResponse(const ServerNetworkMessage<TRequest>& request, double timeoutSeconds)
		{
			std::promise<std::optional<NetworkMessage<TResponse>>> promise;
			ScopedEventListener listener = GetServerSocket().OnMessageReceived().AddScopedEventListener([&promise](Event<ServerMessageReceived>& e)
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
			GetServerSocket().SendPacket(TRequest::Type, request);
			if (timeoutSeconds == IgnoreTimeout)
			{
				std::optional<NetworkMessage<TResponse>> value = future.get();
				return value;
			}
			BLT_ASSERT(timeoutSeconds > 0.0, "Invalid timeout");
			std::future_status status = future.wait_for(std::chrono::nanoseconds((size_t)(timeoutSeconds * 1e9)));
			if (status == std::future_status::ready)
			{
				std::optional<NetworkMessage<TResponse>> value = future.get();
				return value;
			}
			return {};
		}
	};

}