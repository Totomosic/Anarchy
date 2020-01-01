#include "serverpch.h"
#include "Server.h"

#include "Lib/Authentication.h"
#include "ServerState.h"

namespace Anarchy
{

	static constexpr int TARGET_TICK_RATE = 20;
	static const double TARGET_DELTA_TIME = 1.0 / TARGET_TICK_RATE;

	void Server::Init()
	{
		SocketAddress address("localhost", 10000);
		ServerState::Get().Initialize(address);
	}

	void Server::Tick()
	{
	}

	void Server::Update()
	{
		static double prevSleep = 0;
		double delta = Time::Get().RenderingTimeline().DeltaTime() - prevSleep;
		double difference = TARGET_DELTA_TIME - delta;
		if (difference > 0)
		{
			prevSleep = difference;
			std::this_thread::sleep_for(std::chrono::nanoseconds((size_t)(difference * 1e9)));
		}
		BLT_INFO("Tick");
	}

	void Server::Render()
	{
	}

}