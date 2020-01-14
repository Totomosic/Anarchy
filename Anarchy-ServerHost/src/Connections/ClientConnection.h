#pragma once
#include "serverpch.h"
#include "ServerLib.h"

namespace Anarchy
{

	class ClientConnection
	{
	private:
		std::string m_Username;
		connid_t m_ConnectionId;
		SocketAddress m_Address;
		seqid_t m_SequenceId;

	public:
		ClientConnection();
		ClientConnection(const std::string& username, connid_t connectionId, const SocketAddress& address);

		const std::string& GetUsername() const;
		connid_t GetConnectionId() const;
		const SocketAddress& GetAddress() const;
		seqid_t GetSequenceId() const;
		void SetSequenceId(seqid_t sequenceId);

	};

}