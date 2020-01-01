#include "serverpch.h"
#include "ConnectionsManager.h"

namespace Anarchy
{

	ConnectionsManager::ConnectionsManager()
		: m_IdManager(0, std::numeric_limits<uint64_t>::max()), m_Connections()
	{
	}

	const ClientConnection& ConnectionsManager::GetConnection(uint64_t id) const
	{
		return m_Connections.at(id);
	}

	ClientConnection& ConnectionsManager::GetConnection(uint64_t id)
	{
		return m_Connections.at(id);
	}

	std::vector<const ClientConnection*> ConnectionsManager::GetConnections() const
	{
		std::vector<const ClientConnection*> result;
		for (const auto& pair : m_Connections)
		{
			result.push_back(&pair.second);
		}
		return result;
	}

	std::vector<ClientConnection*> ConnectionsManager::GetConnections()
	{
		std::vector<ClientConnection*> result;
		for (auto& pair : m_Connections)
		{
			result.push_back(&pair.second);
		}
		return result;
	}

	ClientConnection& ConnectionsManager::AddConnection(const blt::string& username, const SocketAddress& address)
	{
		uint64_t id = m_IdManager.GetNextId();
		m_Connections[id] = ClientConnection(username, id, address);
		return GetConnection(id);
	}

	bool ConnectionsManager::RemoveConnection(uint64_t id)
	{
		if (m_Connections.find(id) != m_Connections.end())
		{
			m_Connections.erase(id);
			return true;
		}
		return false;
	}

}