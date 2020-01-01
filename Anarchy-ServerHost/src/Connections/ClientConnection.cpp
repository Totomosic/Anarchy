#include "serverpch.h"
#include "ClientConnection.h"

namespace Anarchy
{

	ClientConnection::ClientConnection() : ClientConnection("", InvalidId, {})
	{
	}

	ClientConnection::ClientConnection(const blt::string& username, uint64_t connectionId, const SocketAddress& address)
		: m_Username(username), m_ConnectionId(connectionId), m_Address(address)
	{
	}

	const blt::string& ClientConnection::GetUsername() const
	{
		return m_Username;
	}

	uint64_t ClientConnection::GetConnectionId() const
	{
		return m_ConnectionId;
	}

	const SocketAddress& ClientConnection::GetAddress() const
	{
		return m_Address;
	}

}