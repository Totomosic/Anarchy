#pragma once
#include "serverpch.h"
#include "ServerLib.h"

namespace Anarchy
{

	class ClientConnection
	{
	private:
		blt::string m_Username;
		connid_t m_ConnectionId;
		SocketAddress m_Address;

	public:
		ClientConnection();
		ClientConnection(const blt::string& username, connid_t connectionId, const SocketAddress& address);

		const blt::string& GetUsername() const;
		connid_t GetConnectionId() const;
		const SocketAddress& GetAddress() const;

	};

}