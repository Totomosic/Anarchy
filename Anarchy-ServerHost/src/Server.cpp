#include "serverpch.h"
#include "Server.h"

#include "Lib/Authentication.h"
#include "Lib/GameMessages.h"
#include "ServerState.h"

#include "Utils/Config.h"

#include "Lib/Entities/Components/TilePosition.h"

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
		m_Actions.RegisterHandler<TileMovement>(ActionType::EntityMove, [&entities](const InputAction<TileMovement>& action, bool fromNetwork)
			{
				entities.SetEntityDirty(action.NetworkId);
				EntityHandle entity = entities.GetEntityByNetworkId(action.NetworkId);
				if (entity)
				{
					ComponentHandle position = entity.GetComponent<CTilePosition>();
					position->Position = action.Action.Destination;
				}
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

		ServerEntityCollection& entities = ServerState::Get().GetEntities();
		UpdateEntitiesRequest request;

		for (const GenericAction& action : m_Actions.GetNetworkActions())
		{
			request.Updates.push_back(action);
		}

		m_Actions.ProcessAllActions();
		m_Actions.Clear();
		
		ServerState::Get().GetSocketApi().UpdateEntities(ServerState::Get().GetConnections().GetAllConnectionIds(), request);
		entities.ClearDirtyEntities();
	}

	void Server::Render()
	{
	}

}