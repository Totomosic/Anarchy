#pragma once
#include "ClientConnection.h"

namespace Anarchy
{

	class ConnectionsManager
	{
	private:
		IdManager<uint64_t> m_IdManager;
		std::unordered_map<uint64_t, ClientConnection> m_Connections;

	public:
		ConnectionsManager();

		const ClientConnection& GetConnection(uint64_t id) const;
		ClientConnection& GetConnection(uint64_t id);
		std::vector<const ClientConnection*> GetConnections() const;
		std::vector<ClientConnection*> GetConnections();

		ClientConnection& AddConnection(const blt::string& username, const SocketAddress& address);
		bool RemoveConnection(uint64_t id);
	};

}