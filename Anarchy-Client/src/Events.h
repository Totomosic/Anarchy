#pragma once
#include "Core/Events/Events.h"

namespace Anarchy
{
	
	struct ServerEvents
	{
	public:
		static constexpr uint32_t ServerMessageReceived = Bolt::Events::MIN_USER_ID + 1;
	};

	struct ConnectToServerEvent
	{
	public:
		std::string Username;
		std::string Server;
	};

	struct ClientEvents
	{
	public:
		static constexpr uint32_t ConnectToServer = Bolt::Events::MIN_USER_ID + 100;
		static constexpr uint32_t DisconnectedFromServer = Bolt::Events::MIN_USER_ID + 101;
	};

}