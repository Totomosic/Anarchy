#pragma once
#include "ServerSocket.h"
#include "Lib/Authentication.h"

namespace Anarchy
{

	class ServerListener
	{
	public:
		template<typename T>
		struct ServerMessage
		{
		public:
			MessageType Type;
			SocketAddress From;
			T Data;
		};

	private:
		ScopedEventListener m_Listener;
		ServerSocket& m_ServerSocket;

	public:
		ServerListener(ServerSocket& socket);

		void OnConnectionRequest(const ServerMessage<ServerConnectionRequest>& request);
		void OnDisconnectRequest(const ServerMessage<ServerDisconnectRequest>& request);
	};

}