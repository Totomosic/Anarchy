#pragma once
#include "ClientConnection.h"
#include "Core/Events/IdManager.h"

namespace Anarchy
{

	class ConnectionsManager
	{
	private:
		connid_t m_NextConnectionId;
		std::unordered_map<connid_t, std::unique_ptr<ClientConnection>> m_Connections;

	public:
		ConnectionsManager();

		bool HasConnection(connid_t id) const;

		const ClientConnection& GetConnection(connid_t id) const;
		ClientConnection& GetConnection(connid_t id);
		std::vector<const ClientConnection*> GetConnections() const;
		std::vector<ClientConnection*> GetConnections();
		std::vector<const ClientConnection*> GetConnections(const std::vector<connid_t>& connectionIds) const;
		std::vector<ClientConnection*> GetConnections(const std::vector<connid_t>& connectionIds);

		std::vector<connid_t> GetAllConnectionIds() const;
		std::vector<connid_t> GetConnectionIdsExcept(connid_t connectionId) const;

		ClientConnection& AddConnection(const std::string& username, const SocketAddress& address);
		bool RemoveConnection(connid_t id);

	private:
		connid_t GetNextConnectionId();
	};

}