#pragma once
#include "Connections/ConnectionsManager.h"
#include "ServerSocket.h"
#include "ServerListener.h"
#include "Entities/ServerEntityCollection.h"

#include "Debug/DebugCommandManager.h"

namespace Anarchy
{

	class ServerState
	{
	private:
		std::unique_ptr<ServerSocket> m_Socket;
		std::unique_ptr<ServerListener> m_Listener;
		ConnectionsManager m_Connections;

		std::unique_ptr<ServerEntityCollection> m_Entities;

		DebugCommandManager m_DebugCommands;

	private:
		ServerState();

	public:
		static ServerState& Get();

	public:
		void Initialize(const SocketAddress& serverAddress, Scene& gameScene, Layer& gameLayer);

		const ServerSocket& GetSocket() const;
		ServerSocket& GetSocket();
		const ConnectionsManager& GetConnections() const;
		ConnectionsManager& GetConnections();
		const ServerListener& GetSocketApi() const;
		ServerListener& GetSocketApi();

		const ServerEntityCollection& GetEntities() const;
		ServerEntityCollection& GetEntities();

	};

}