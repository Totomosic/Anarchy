#include "clientpch.h"
#include "Client.h"

#include "Scenes.h"
#include "ClientState.h"
#include "Lib/Entities/ActionExecutor.h"

namespace Anarchy
{

	void Client::Init()
	{
		GetWindow().DisableVSync();
		Scene& titleScene = SceneManager::Get().AddScene();
		Scene& gameScene = SceneManager::Get().AddScene();

		CreateTitleScene(titleScene, GetWindow(), gameScene);
		CreateGameScene(gameScene, GetWindow());
		SceneManager::Get().SetCurrentScene(titleScene);
	}

	void Client::Tick()
	{
		
	}

	void Client::Update()
	{
		if (ClientState::Get().HasConnection())
		{
			ConnectionManager& connection = ClientState::Get().GetConnection();
			connection.Update(Time::Get().RenderingTimeline().DeltaTime());
			if (connection.IsConnected())
			{
				ActionHistory& actionHistory = ClientState::Get().GetActionHistory();
				ActionRegistry& actionRegistry = ClientState::Get().GetActionRegistry();
				ClientEntityCollection& entities = ClientState::Get().GetEntities();
				const std::unordered_map<entityid_t, EntityUpdate>& entityUpdates = connection.GetSocketApi().GetReceivedEntityUpdates();
				if (!entityUpdates.empty())
				{
					ActionExecutor executor;
					for (const auto& pair : entityUpdates)
					{
						entityid_t entityId = pair.first;
						const EntityUpdate& update = pair.second;
						EntityHandle entity = entities.GetEntityByNetworkId(entityId);
						if (entity)
						{
							if (entities.IsControllingEntity(entityId))
							{
								if (update.MaxActionId)
								{
									actionHistory.ClearActionsBeforeIncluding(update.MaxActionId.value());
								}
								EntityState state = executor.ApplyActions(update.FinalState, actionHistory.GetAllActions());
								EntityState current = entities.GetStateFromEntity(entity);
								if (state != current)
								{
									entities.ApplyEntityState(update.FinalState);
									actionHistory.ClearAllActions();
									BLT_WARN("Applied Server State");
								}
							}
							else
							{
								EntityState initialState = entities.GetStateFromEntity(entity);
								actionRegistry.ApplyActions(update.Actions);
								EntityState state = executor.ApplyActions(initialState, update.Actions);
								if (state != update.FinalState)
								{
									entities.ApplyEntityState(update.FinalState);
									BLT_WARN("Applied Server State");
								}
							}
						}
					}
					connection.GetSocketApi().ClearReceivedEntityUpdates();
				}

				if (Input::Get().KeyPressed(Keycode::Esc))
				{
					connection.GetSocketApi().Disconnect({}, 2.0).ContinueWithOnMainThread([](std::optional<ServerDisconnectResponse> response)
						{
							ClientState::Get().CloseConnection();
						});
				}
			}
		}
		if (ClientState::Get().HasTilemap() && ClientState::Get().HasEntities())
		{
			EntityHandle camera = ClientState::Get().GetEntities().GetCamera();
			if (camera)
			{
				ComponentHandle transform = camera.GetTransform();
				ClientState::Get().GetTilemap().LoadTilePosition((int64_t)transform->Position().x, (int64_t)transform->Position().y);
			}
		}
	}

	void Client::Render()
	{
		Graphics::Get().RenderScene();
	}

	void Client::Exit()
	{
		if (ClientState::Get().HasConnection())
		{
			if (ClientState::Get().GetConnection().IsConnected())
			{
				ClientState::Get().GetConnection().GetSocketApi().Disconnect({}, 5.0).Wait();
			}
			ClientState::Get().CloseConnection();
		}
		ClientState::Terminate();
		Application::Exit();
	}

}