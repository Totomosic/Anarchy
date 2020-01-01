#include "serverpch.h"
#include "ServerState.h"

namespace Anarchy
{

	ServerState::ServerState()
		: m_Socket(), m_Connections()
	{
	}

	ServerState& ServerState::Get()
	{
		static ServerState instance;
		return instance;
	}

	void ServerState::Initialize(const SocketAddress& serverAddress)
	{
		m_Socket = std::make_unique<ServerSocket>(serverAddress);
		m_Socket->Run();
		BLT_INFO("Server starting at {}", serverAddress);
		m_Listener = std::make_unique<ServerListener>(*m_Socket);
	}

	const ServerSocket& ServerState::GetSocket() const
	{
		return *m_Socket;
	}

	ServerSocket& ServerState::GetSocket()
	{
		return *m_Socket;
	}

	const ConnectionsManager& ServerState::GetConnections() const
	{
		return m_Connections;
	}

	ConnectionsManager& ServerState::GetConnections()
	{
		return m_Connections;
	}

}