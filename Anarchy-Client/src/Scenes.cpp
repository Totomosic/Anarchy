#include "clientpch.h"
#include "Scenes.h"
#include "Events.h"

#include "ClientState.h"
#include "Entities/Systems/PlayerControlSystem.h"
#include "Entities/Systems/MovementSystem.h"

#include "Lib/Entities/Components/TilePosition.h"
#include "Entities/Components/TileMotion.h"

namespace Anarchy
{

	void CreateTitleScene(Scene& scene, const Window& window, Scene& gameScene)
	{
		Layer& layer = scene.AddLayer();
		EntityFactory factory = layer.GetFactory();
		EntityHandle camera = factory.Camera(Matrix4f::Orthographic(0, window.Width(), 0, window.Height(), -100, 100));
		layer.SetActiveCamera(camera);

		ResourcePtr<Font> titleFont = ResourceManager::Get().Fonts().Calibri(36);
		ResourcePtr<Font> displayFont = ResourceManager::Get().Fonts().Calibri(18);

		UIElement& ui = layer.GetUI().GetRoot();
		UIRectangle& background = ui.CreateRectangle(500, 600, Color::White, Transform({ window.Width() / 2.0f, window.Height() / 2.0f, 0 }));
		background.CreateText("Anarchy", titleFont, Color::Black, Transform({ 0, 125, 1 }));

		UITextInput& serverInput = background.CreateTextInput(300, 50, displayFont, Color::Black, Color(128, 128, 128), Transform({ 0, 0, 1 }));
		serverInput.CreateText("Server:", displayFont, Color::Black, Transform({ -150, 30, 1 }), AlignH::Left, AlignV::Bottom);
		serverInput.SetText("localhost:10000");

		UIRectangle& connectButton = background.CreateRectangle(300, 50, Color(50, 200, 50), Transform({ 0, -60, 1 }));
		connectButton.CreateText("Connect", displayFont, Color::Black, Transform({ 0, 0, 1 }));
		connectButton.Events().OnClick().AddEventListener([&background, &serverInput, &gameScene, &scene](Event<UI<MouseClickEvent>>& e)
			{
				if (!ClientState::Get().HasConnection() || (!ClientState::Get().GetConnection().IsConnected() && !ClientState::Get().GetConnection().IsConnecting()))
				{
					ConnectToServerEvent data;
					data.Username = "Totomosic";
					data.Server = serverInput.GetText();

					size_t colon = data.Server.rfind(':');
					std::string host = data.Server.substr(0, colon);
					std::string port = data.Server.substr(colon + 1);
					SocketAddress address(host, port);
					ClientState::Get().InitializeConnection(address);
					ClientState::Get().GetConnection().GetSocketApi().OnDisconnect().AddEventListener([&scene](Event<ServerDisconnect>& e)
						{
							SceneManager::Get().SetCurrentScene(scene);
							e.StopPropagation();
						});

					ServerConnectionRequest request;
					request.Username = data.Username;

					UIRectangle& connectingIcon = background.CreateRectangle(30, 30, Color::Red, Transform({ 170, -60, 1 }));

					Task<std::optional<ServerConnectionResponse>> response = ClientState::Get().GetConnection().GetSocketApi().Connect(request, 2.0);
					response.ContinueWithOnMainThread([&gameScene, &connectingIcon](std::optional<ServerConnectionResponse> response)
						{
							connectingIcon.Remove();
							if (response)
							{
								SceneManager::Get().SetCurrentScene(gameScene);
							}
							else
							{
								BLT_WARN("Connection Failed");
							}
						});
				}
			});

		background.CreateText("Connecting As: Totomosic", displayFont, Color::Black, Transform({ 0, -100, 1 }));
	}

	void CreateGameScene(Scene& scene, const Window& window)
	{
		
		scene.OnLoad().AddEventListener([&scene](Event<SceneLoadEvent>& e)
			{
				CreateCharacterRequest request;
				std::optional<CreateCharacterResponse> character = ClientState::Get().GetConnection().GetSocketApi().CreateCharacter(request, 5.0).Result();
				if (character)
				{
					int width = 50;
					int height = 50;
					EntityHandle camera = scene.GetFactory().Camera(Matrix4f::Orthographic(-16, 16, -9, 9, -100, 100));
					Layer& mapLayer = scene.AddLayer();
					Layer& gameLayer = scene.AddLayer();
					mapLayer.SetActiveCamera(camera);
					gameLayer.SetActiveCamera(camera);

					BLT_INFO("Created Character Successfully");
					
					ClientState::Get().InitializeTilemap(scene, mapLayer, width, height);
					ClientState::Get().InitializeEntities(scene, gameLayer);
					ClientEntityCollection& entities = ClientState::Get().GetEntities();

					ClientState::Get().GetTilemap().SetTiles(0, 0, width, height, TileType::Grass);

					ClientState::Get().GetConnection().GetSocketApi().GetTilemap({ 0, 160, 330, width, height }).ContinueWithOnMainThread([](std::optional<GetTilemapResponse>& response)
						{
							if (response)
							{
								ClientState::Get().GetTilemap().SetTiles(0, 0, response->Width, response->Height, response->Tiles.data());
								ClientState::Get().GetTilemapRenderer().Invalidate();
							}
						});

					EntityHandle player = entities.CreateFromEntityData(character->Data);
					ComponentHandle controller = player.Assign<CPlayerController>();
					controller->Speed = 10.0f;

					ActionBuffer& commands = ClientState::Get().GetConnection().GetSocketApi().GetActionBuffer();
					auto movementSystem = gameLayer.Systems().Add<MovementSystem>();
					auto controlSystem = gameLayer.Systems().Add<PlayerControlSystem>(&commands);

					entities.SetCamera(camera);
					entities.SetOwnedEntity(character->Data.NetworkId);

					commands.RegisterHandler<TileMovement>(ActionType::EntityMove, [&entities](const InputAction<TileMovement>& action, bool fromNetwork)
						{
							if (!fromNetwork || !entities.OwnsEntity(action.NetworkId))
							{
								EntityHandle entity = entities.GetEntityByNetworkId(action.NetworkId);
								if (entity && entity.HasComponent<CTilePosition>())
								{
									ComponentHandle position = entity.GetComponent<CTilePosition>();
									CTileMotion motion;
									motion.Destination = action.Action.Destination;
									motion.Speed = action.Action.Speed;
									position->Position = action.Action.Destination;
									entity.Assign<CTileMotion>(std::move(motion));
								}
							}
						});

					std::optional<GetEntitiesResponse> otherEntities = ClientState::Get().GetConnection().GetSocketApi().GetEntities({ 0 }, 5.0).Result();
					if (otherEntities)
					{
						for (const EntityData& data : otherEntities->Entities)
						{
							if (data.NetworkId != character->Data.NetworkId)
							{
								entities.CreateFromEntityData(data);
							}
						}
					}
				}
			});
		scene.OnUnload().AddEventListener([&scene](Event<SceneUnloadEvent>& e)
			{
				ClientState::Get().DestroyEntities();
				scene.Clear();
			});
	}

}