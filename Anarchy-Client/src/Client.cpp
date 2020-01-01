#include "clientpch.h"
#include "Client.h"

#include "ServerConnection.h"

namespace Anarchy
{

	void Client::Init()
	{
		SocketAddress serverAddr("localhost", 10000);
		ServerConnection connection(serverAddr);

		ServerConnectionResponse response = connection.Connect({}).Result();
		BLT_INFO(response.Success);
	}

	void Client::Tick()
	{
	}

	void Client::Update()
	{
	}

	void Client::Render()
	{
		Graphics::Get().RenderScene();
	}

}