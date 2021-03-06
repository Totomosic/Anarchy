#include "serverpch.h"
#include "ServerState.h"

namespace Anarchy
{

	extern Bolt::DirectoryPath WorldDirectory;

	ServerState::ServerState()
		: m_TargetDeltaTime(1.0 / 20.0), m_Socket(), m_Connections(), m_Entities(), m_DebugCommands(), m_WorldReader(WorldDirectory)
	{
		BLT_INFO("Loading world from {}", WorldDirectory);
	}

	ServerState& ServerState::Get()
	{
		static ServerState instance;
		return instance;
	}

	void ServerState::Initialize(double targetDeltaTime, const SocketAddress& serverAddress, Scene& gameScene, Layer& gameLayer)
	{
		m_TargetDeltaTime = targetDeltaTime;
		m_Entities = std::make_unique<ServerEntityCollection>(gameScene, gameLayer);
		m_Socket = std::make_unique<ServerSocket>(serverAddress);
		m_Socket->Run();
		BLT_INFO("Server starting at {}", serverAddress);
		m_Listener = std::make_unique<ServerListener>(*m_Socket);

		m_DebugCommands.StartStdinListener();
	}

	double ServerState::GetTargetDeltaTime() const
	{
		return m_TargetDeltaTime;
	}

	double ServerState::GetTargetTicksPerSecond() const
	{
		return 1.0 / m_TargetDeltaTime;
	}

	void ServerState::SetTargetTicksPerSecond(int tps)
	{
		m_TargetDeltaTime = 1.0 / (double)tps;
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

	const ServerListener& ServerState::GetSocketApi() const
	{
		return *m_Listener;
	}

	ServerListener& ServerState::GetSocketApi()
	{
		return *m_Listener;
	}

	const ServerEntityCollection& ServerState::GetEntities() const
	{
		return *m_Entities;
	}

	ServerEntityCollection& ServerState::GetEntities()
	{
		return *m_Entities;
	}

	const WorldReader& ServerState::GetWorld() const
	{
		return m_WorldReader;
	}

	WorldReader& ServerState::GetWorld()
	{
		return m_WorldReader;
	}

}