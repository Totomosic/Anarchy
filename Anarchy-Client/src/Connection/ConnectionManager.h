#pragma once
#include "ServerConnection.h"
#include "Lib/SocketApi.h"

namespace Anarchy
{

	class ConnectionManager : public ClientSocketApi
	{
	public:
		inline static const double IgnoreTimeout = -1.0;

	private:
		EventBus m_Bus;
		TaskManager m_TaskManager;

		ServerConnection m_Connection;
		connid_t m_ConnectionId;
		bool m_Connecting;
		seqid_t m_SequenceId;

		ScopedEventListener m_Listener;
		std::unordered_map<MessageType, std::function<void(InputMemoryStream&)>> m_MessageHandlers;

	public:
		ConnectionManager(const SocketAddress& address);
		~ConnectionManager();

		bool IsConnecting() const;
		bool IsConnected() const;
		const ServerConnection& GetServerSocket() const;
		ServerConnection& GetServerSocket();

		connid_t GetConnectionId() const;

		Promise<ServerConnectionResponse> Connect(const ServerConnectionRequest& request, double timeoutSeconds = IgnoreTimeout) override;
		Promise<ServerDisconnectResponse> Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds = IgnoreTimeout) override;

		Promise<CreateCharacterResponse> CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds = IgnoreTimeout) override;
		Promise<GetEntitiesResponse> GetEntities(const GetEntitiesRequest& request, double timeoutSeconds = IgnoreTimeout) override;

		void SpawnEntities(const NetworkMessage<SpawnEntitiesRequest>& request) override;
		void DestroyEntities(const NetworkMessage<DestroyEntitiesRequest>& request) override;
		void UpdateEntities(const NetworkMessage<UpdateEntitiesRequest>& request) override;

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
		template<typename T>
		ServerNetworkMessage<T> CreateMessage(const T& data)
		{
			ServerNetworkMessage<T> message;
			message.ConnectionId = GetConnectionId();
			message.Message = data;
			message.SequenceId = m_SequenceId++;
			return message;
		}

		template<typename TResponse, typename TRequest>
		std::optional<TResponse> AwaitResponse(const ServerNetworkMessage<TRequest>& request, double timeoutSeconds)
		{
			std::promise<std::optional<TResponse>> promise;
			ScopedEventListener listener = GetServerSocket().OnMessageReceived().AddScopedEventListener([&promise](Event<ServerMessageReceived>& e)
				{
					if (e.Data.Type == TResponse::Type)
					{
						std::optional<NetworkMessage<TResponse>> response;
						Deserialize(e.Data.Data, response);
						if (response.has_value())
						{
							promise.set_value(std::move(response->Message));
						}
						else
						{
							promise.set_value({});
						}
					}
				});
			std::future<std::optional<TResponse>> future = promise.get_future();
			GetServerSocket().SendPacket(TRequest::Type, request);
			if (timeoutSeconds == IgnoreTimeout)
			{
				std::optional<TResponse> value = future.get();
				return value;
			}
			BLT_ASSERT(timeoutSeconds > 0.0, "Invalid timeout");
			std::future_status status = future.wait_for(std::chrono::nanoseconds((size_t)(timeoutSeconds * 1e9)));
			if (status == std::future_status::ready)
			{
				std::optional<TResponse> value = future.get();
				return value;
			}
			return {};
		}
	};

}