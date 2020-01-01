#pragma once
#include "ServerConnection.h"

namespace Anarchy
{

	class ConnectionManager
	{
	private:
		static std::unique_ptr<ConnectionManager> s_Instance;

	private:
		std::unique_ptr<ServerConnection> m_Connection;
		uint64_t m_ConnectionId;

	public:
		static ConnectionManager& Get();
		static void Terminate();

	public:
		ConnectionManager();

		void Initialize(const SocketAddress& address);

		const ServerConnection& GetConnection() const;
		ServerConnection& GetConnection();
		void CloseConnection();

		uint64_t GetConnectionId() const;
		void SetConnectionId(uint64_t id);
	};

}