#include "clientpch.h"
#include "ConnectionManager.h"

namespace Anarchy
{

	std::unique_ptr<ConnectionManager> ConnectionManager::s_Instance;

	ConnectionManager::ConnectionManager()
		: m_Connection(nullptr), m_ConnectionId(InvalidConnectionId)
	{

	}

	ConnectionManager& ConnectionManager::Get()
	{
		if (!s_Instance)
		{
			s_Instance = std::make_unique<ConnectionManager>();
		}
		return *s_Instance;
	}

	void ConnectionManager::Terminate()
	{
		s_Instance = nullptr;
	}

	void ConnectionManager::Initialize(const SocketAddress& address)
	{
		if (m_Connection != nullptr)
		{
			Disconnect({ GetConnectionId() }, IgnoreTimeout).Wait();
		}
		m_Connection = std::make_unique<ServerConnection>(address);
	}

	bool ConnectionManager::HasConnection() const
	{
		return m_Connection != nullptr;
	}

	const ServerConnection& ConnectionManager::GetConnection() const
	{
		return *m_Connection;
	}

	ServerConnection& ConnectionManager::GetConnection()
	{
		return *m_Connection;
	}

	void ConnectionManager::CloseConnection()
	{
		m_Connection = nullptr;
	}

	connid_t ConnectionManager::GetConnectionId() const
	{
		return m_ConnectionId;
	}

	void ConnectionManager::SetConnectionId(connid_t id)
	{
		m_ConnectionId = id;
	}

	ClientSocketApi::Promise<ServerConnectionResponse> ConnectionManager::Connect(const ServerConnectionRequest& request, double timeoutSeconds)
	{
		return TaskManager::Run([this, request, timeoutSeconds]()
			{
				return AwaitResponse<ServerConnectionResponse>(request, timeoutSeconds);
			});
	}

	ClientSocketApi::Promise<ServerDisconnectResponse> ConnectionManager::Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds)
	{
		return TaskManager::Run([this, request, timeoutSeconds]()
			{
				return AwaitResponse<ServerDisconnectResponse>(request, timeoutSeconds);
			});
	}

	ClientSocketApi::Promise<CreateCharacterResponse> ConnectionManager::CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds)
	{
		return TaskManager::Run([this, request, timeoutSeconds]()
			{
				return AwaitResponse<CreateCharacterResponse>(request, timeoutSeconds);
			});
	}

}