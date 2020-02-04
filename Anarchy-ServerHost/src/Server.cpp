#include "serverpch.h"
#include "Server.h"

#include "Lib/Authentication.h"
#include "Lib/GameMessages.h"
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
		ServerState::Get().Initialize(ServerAddress, gameScene, gameLayer);
		ServerState::Get().GetSocketApi().SetActionBuffer(&m_Actions);

		ServerEntityCollection& entities = ServerState::Get().GetEntities();
		m_Actions.RegisterHandler<TileMovement>(ActionType::EntityMove, [&entities](const InputAction<TileMovement>& command)
			{
				entities.SetEntityDirty(command.NetworkId);
			});
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

		m_Actions.ProcessAllActions();

		ServerEntityCollection& entities = ServerState::Get().GetEntities();
		const std::vector<entityid_t> dirtyEntities = entities.GetDirtyEntities();
		UpdateEntitiesRequest request;
		for (entityid_t entity : dirtyEntities)
		{
			EntityHandle e = entities.GetEntityByNetworkId(entity);
			EntityData data = entities.GetDataFromEntity(e);
			EntityDelta delta;
			delta.NetworkId = entity;
			delta.TilePosition = data.TilePosition;
			request.Updates.push_back(delta);
		}
		ServerState::Get().GetSocketApi().UpdateEntities(ServerState::Get().GetConnections().GetAllConnectionIds(), request);
		entities.ClearDirtyEntities();
	}

	void Server::Render()
	{
	}

}