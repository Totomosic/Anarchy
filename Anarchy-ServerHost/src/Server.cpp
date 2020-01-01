#include "serverpch.h"
#include "Server.h"

#include "Lib/Authentication.h"

namespace Anarchy
{

	void Server::Init()
	{
		SocketAddress address("localhost", 10000);
		m_Socket = std::make_unique<ServerSocket>(address);

		m_Socket->OnMessageReceived().AddEventListener([this](Event<ClientMessageReceived>& e)
			{
				BLT_INFO("Message Received from {}", e.Data.From);
				if (e.Data.Type == MessageType::ConnectRequest)
				{
					ServerConnectionResponse response;
					response.Success = true;
					response.PlayerId = 0;
					m_Socket->SendPacket(e.Data.From, MessageType::ConnectResponse, response);
				}
			});

		m_Socket->Run();
	}

	void Server::Tick()
	{
	}

	void Server::Update()
	{
	}

	void Server::Render()
	{
	}

}