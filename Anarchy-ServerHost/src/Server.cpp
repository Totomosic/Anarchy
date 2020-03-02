#include "serverpch.h"
#include "Server.h"

#include "Lib/World/Reader/WorldReader.h"
#include "Lib/RequestMessages.h"
#include "Lib/GameMessages.h"
#include "ServerState.h"
#include "Lib/Entities/ActionExecutor.h"
#include "Lib/Entities/Components/NetworkId.h"

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
		ServerState::Get().GetSocketApi().SetActionQueue(&m_Actions);
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
		ActionExecutor executor;
		UpdateEntitiesRequest request;
		std::unordered_map<entityid_t, std::vector<GenericAction>> actions;
		for (const GenericAction& action : m_Actions.GetAllActions())
		{
			actions[action.NetworkId].push_back(action);
		}
		for (auto& pair : actions)
		{
			EntityHandle entity = entities.GetEntityByNetworkId(pair.first);
			if (entity)
			{
				std::optional<seqid_t> maxActionId;
				if (pair.second.size() > 0)
				{
					maxActionId = pair.second[0].ActionId;
					for (const GenericAction& action : pair.second)
					{
						if (IsSeqIdGreater(action.ActionId, maxActionId.value()))
						{
							maxActionId = action.ActionId;
						}
					}
				}
				EntityState initialState = entities.GetStateFromEntity(entity);
				EntityState finalState = executor.ApplyActions(initialState, pair.second);
				WorldReader& world = ServerState::Get().GetWorld();
				if (world.GetTile(finalState.TilePosition.x, finalState.TilePosition.y) == TileType::Water)
				{
					finalState = initialState;
					pair.second.clear();
				}
				entities.ApplyEntityState(finalState);
				request.Updates.push_back({ finalState, pair.second, maxActionId });
			}
		}
		m_Actions.Clear();
		for (EntityHandle entity : entities.GetAllEntities())
		{
			if (entity)
			{
				entityid_t networkId = entity.GetComponent<CNetworkId>()->Id;
				if (actions.find(networkId) == actions.end())
				{
					request.Updates.push_back({ entities.GetStateFromEntity(entity), {} });
				}
			}
		}
		
		ServerState::Get().GetSocketApi().UpdateEntities(ServerState::Get().GetConnections().GetAllConnectionIds(), request);
		entities.ClearDirtyEntities();
	}

	void Server::Render()
	{
	}

}