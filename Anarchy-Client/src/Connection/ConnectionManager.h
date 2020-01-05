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
		static std::unique_ptr<ConnectionManager> s_Instance;

	private:
		std::unique_ptr<ServerConnection> m_Connection;
		connid_t m_ConnectionId;

	public:
		static ConnectionManager& Get();
		static void Terminate();

	public:
		ConnectionManager();

		void Initialize(const SocketAddress& address);

		bool HasConnection() const;
		const ServerConnection& GetConnection() const;
		ServerConnection& GetConnection();
		void CloseConnection();

		connid_t GetConnectionId() const;
		void SetConnectionId(connid_t id);

		Promise<ServerConnectionResponse> Connect(const ServerConnectionRequest& request, double timeoutSeconds = IgnoreTimeout) override;
		Promise<ServerDisconnectResponse> Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds = IgnoreTimeout) override;

		Promise<CreateCharacterResponse> CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds = IgnoreTimeout) override;
		
	private:
		template<typename TResponse, typename TRequest>
		std::optional<TResponse> AwaitResponse(const TRequest& request, double timeoutSeconds)
		{
			std::promise<TResponse> promise;
			ScopedEventListener listener = GetConnection().OnMessageReceived().AddScopedEventListener([&promise](Event<ServerMessageReceived>& e)
				{
					if (e.Data.Type == TResponse::Type)
					{
						TResponse response;
						Deserialize(e.Data.Data, response);
						promise.set_value(std::move(response));
					}
				});
			std::future<TResponse> future = promise.get_future();
			GetConnection().SendPacket(TRequest::Type, request);
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