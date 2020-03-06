#pragma once
#include "Networking/Connections/ConnectionsManager.h"
#include "Networking/ServerSocket.h"
#include "Networking/ServerListener.h"
#include "Entities/ServerEntityCollection.h"

#include "Debug/DebugCommandManager.h"
#include "Lib/World/Reader/WorldReader.h"

namespace Anarchy
{

	class ServerState
	{
	private:
		double m_TargetDeltaTime;

		std::unique_ptr<ServerSocket> m_Socket;
		std::unique_ptr<ServerListener> m_Listener;
		ConnectionsManager m_Connections;

		std::unique_ptr<ServerEntityCollection> m_Entities;
		WorldReader m_WorldReader;

		DebugCommandManager m_DebugCommands;

	private:
		ServerState();

	public:
		static ServerState& Get();

	public:
		void Initialize(double targetDeltaTime, const SocketAddress& serverAddress, Scene& gameScene, Layer& gameLayer);

		double GetTargetDeltaTime() const;
		double GetTargetTicksPerSecond() const;
		void SetTargetTicksPerSecond(int tps);

		const ServerSocket& GetSocket() const;
		ServerSocket& GetSocket();
		const ConnectionsManager& GetConnections() const;
		ConnectionsManager& GetConnections();
		const ServerListener& GetSocketApi() const;
		ServerListener& GetSocketApi();

		const ServerEntityCollection& GetEntities() const;
		ServerEntityCollection& GetEntities();
		const WorldReader& GetWorld() const;
		WorldReader& GetWorld();

	};

}