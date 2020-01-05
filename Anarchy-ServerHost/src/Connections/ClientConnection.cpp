#include "serverpch.h"
#include "ClientConnection.h"

namespace Anarchy
{

	ClientConnection::ClientConnection() : ClientConnection("", InvalidConnectionId, {})
	{
	}

	ClientConnection::ClientConnection(const blt::string& username, connid_t connectionId, const SocketAddress& address)
		: m_Username(username), m_ConnectionId(connectionId), m_Address(address)
	{
	}

	const blt::string& ClientConnection::GetUsername() const
	{
		return m_Username;
	}

	connid_t ClientConnection::GetConnectionId() const
	{
		return m_ConnectionId;
	}

	const SocketAddress& ClientConnection::GetAddress() const
	{
		return m_Address;
	}

}