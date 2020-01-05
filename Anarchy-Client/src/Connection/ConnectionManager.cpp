#include "clientpch.h"
#include "ConnectionManager.h"

namespace Anarchy
{

	ConnectionManager::ConnectionManager(const SocketAddress& address)
		: m_Connection(address), m_ConnectionId(InvalidConnectionId), m_Connecting(false)
	{

	}

	bool ConnectionManager::IsConnecting() const
	{
		return m_Connecting;
	}

	bool ConnectionManager::IsConnected() const
	{
		return m_ConnectionId != InvalidConnectionId;
	}

	const ServerConnection& ConnectionManager::GetServerSocket() const
	{
		return m_Connection;
	}

	ServerConnection& ConnectionManager::GetServerSocket()
	{
		return m_Connection;
	}

	connid_t ConnectionManager::GetConnectionId() const
	{
		return m_ConnectionId;
	}

	ClientSocketApi::Promise<ServerConnectionResponse> ConnectionManager::Connect(const ServerConnectionRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(!IsConnected() && !IsConnecting(), "Cannot connect now");
		m_Connecting = true;
		return TaskManager::Run([this, request, timeoutSeconds]()
			{
				auto response = AwaitResponse<ServerConnectionResponse>(request, timeoutSeconds);
				m_Connecting = false;
				if (response && response->Success)
				{
					m_ConnectionId = response->ConnectionId;
				}
				return response;
			});
	}

	ClientSocketApi::Promise<ServerDisconnectResponse> ConnectionManager::Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected() && !IsConnecting(), "Cannot disconnect now");
		return TaskManager::Run([this, request, timeoutSeconds]()
			{
				auto response = AwaitResponse<ServerDisconnectResponse>(request, timeoutSeconds);
				if (response && response->Success)
				{
					m_ConnectionId = InvalidConnectionId;
				}
				return response;
			});
	}

	ClientSocketApi::Promise<CreateCharacterResponse> ConnectionManager::CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected(), "Must be connected");
		return TaskManager::Run([this, request, timeoutSeconds]()
			{
				return AwaitResponse<CreateCharacterResponse>(request, timeoutSeconds);
			});
	}

	ClientSocketApi::Promise<GetEntitiesResponse> ConnectionManager::GetEntities(const GetEntitiesRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected(), "Must be connected");
		return TaskManager::Run([this, request, timeoutSeconds]()
			{
				return AwaitResponse<GetEntitiesResponse>(request, timeoutSeconds);
			});
	}

}