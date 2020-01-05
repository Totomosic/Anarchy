#pragma once
#include "ClientConnection.h"

namespace Anarchy
{

	class ConnectionsManager
	{
	private:
		IdManager<connid_t> m_IdManager;
		std::unordered_map<connid_t, ClientConnection> m_Connections;

	public:
		ConnectionsManager();

		bool HasConnection(connid_t id) const;
		const ClientConnection& GetConnection(connid_t id) const;
		ClientConnection& GetConnection(connid_t id);
		std::vector<const ClientConnection*> GetConnections() const;
		std::vector<ClientConnection*> GetConnections();

		ClientConnection& AddConnection(const blt::string& username, const SocketAddress& address);
		bool RemoveConnection(connid_t id);
	};

}