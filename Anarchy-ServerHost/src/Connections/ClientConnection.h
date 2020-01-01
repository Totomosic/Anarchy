#pragma once
#include "serverpch.h"

namespace Anarchy
{

	class ClientConnection
	{
	public:
		static constexpr uint64_t InvalidId = (uint64_t)-1;
		
	private:
		blt::string m_Username;
		uint64_t m_ConnectionId;
		SocketAddress m_Address;

	public:
		ClientConnection();
		ClientConnection(const blt::string& username, uint64_t connectionId, const SocketAddress& address);

		const blt::string& GetUsername() const;
		uint64_t GetConnectionId() const;
		const SocketAddress& GetAddress() const;

	};

}