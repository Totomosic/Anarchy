#include "serverpch.h"
#include "Server.h"

#include "Lib/Authentication.h"
#include "ServerState.h"

#include "Utils/Config.h"

namespace Anarchy
{

	static constexpr int TARGET_TICK_RATE = 20;
	static constexpr double TARGET_DELTA_TIME = 1.0 / TARGET_TICK_RATE;

	void Server::Init()
	{
		Scene& gameScene = SceneManager::Get().AddScene();
		Layer& gameLayer = gameScene.AddLayer();
		SocketAddress address = LoadServerConfig();
		ServerState::Get().Initialize(address, gameScene, gameLayer);
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

		ServerState::Get().GetSocketApi().Update(Time::Get().RenderingTimeline().DeltaTime());

		UpdateEntitiesRequest request;
		ServerState::Get().GetSocketApi().UpdateEntities(ServerState::Get().GetConnections().GetAllConnectionIds(), request);
	}

	void Server::Render()
	{
	}

}