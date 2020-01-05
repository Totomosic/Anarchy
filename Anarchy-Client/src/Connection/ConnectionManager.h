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
		ServerConnection m_Connection;
		connid_t m_ConnectionId;
		bool m_Connecting;

	public:
		ConnectionManager(const SocketAddress& address);

		bool IsConnecting() const;
		bool IsConnected() const;
		const ServerConnection& GetServerSocket() const;
		ServerConnection& GetServerSocket();

		connid_t GetConnectionId() const;

		Promise<ServerConnectionResponse> Connect(const ServerConnectionRequest& request, double timeoutSeconds = IgnoreTimeout) override;
		Promise<ServerDisconnectResponse> Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds = IgnoreTimeout) override;

		Promise<CreateCharacterResponse> CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds = IgnoreTimeout) override;
		Promise<GetEntitiesResponse> GetEntities(const GetEntitiesRequest& request, double timeoutSeconds = IgnoreTimeout) override;
		
	private:
		template<typename TResponse, typename TRequest>
		std::optional<TResponse> AwaitResponse(const TRequest& request, double timeoutSeconds)
		{
			std::promise<TResponse> promise;
			ScopedEventListener listener = GetServerSocket().OnMessageReceived().AddScopedEventListener([&promise](Event<ServerMessageReceived>& e)
				{
					if (e.Data.Type == TResponse::Type)
					{
						TResponse response;
						Deserialize(e.Data.Data, response);
						promise.set_value(std::move(response));
					}
				});
			std::future<TResponse> future = promise.get_future();
			GetServerSocket().SendPacket(TRequest::Type, request);
			if (timeoutSeconds == IgnoreTimeout)
			{
				return future.get();
			}
			BLT_ASSERT(timeoutSeconds > 0.0, "Invalid timeout");
			std::future_status status = future.wait_for(std::chrono::nanoseconds((size_t)(timeoutSeconds * 1e9)));
			if (status == std::future_status::ready)
			{
				return future.get();
			}
			return {};
		}
	};

}