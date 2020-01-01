#include "clientpch.h"
#include "ConnectionManager.h"

namespace Anarchy
{

	std::unique_ptr<ConnectionManager> ConnectionManager::s_Instance;

	ConnectionManager::ConnectionManager()
		: m_Connection(nullptr), m_ConnectionId(0)
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
			m_Connection->RequestDisconnect(GetConnectionId());
		}
		m_Connection = std::make_unique<ServerConnection>(address);
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

	uint64_t ConnectionManager::GetConnectionId() const
	{
		return m_ConnectionId;
	}

	void ConnectionManager::SetConnectionId(uint64_t id)
	{
		m_ConnectionId = id;
	}

}