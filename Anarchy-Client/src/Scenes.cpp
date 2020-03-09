#include "clientpch.h"
#include "Scenes.h"
#include "Events.h"

#include "ClientState.h"
#include "Entities/Systems/PlayerControlSystem.h"
#include "Entities/Systems/MovementSystem.h"
#include "Entities/Systems/TileIndicatorSystem.h"

#include "Lib/Entities/Components/TilePosition.h"
#include "Entities/Components/TileMotion.h"

#include "World/Tilemap.h"

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

		UITextInput& usernameInput = background.CreateTextInput(300, 50, displayFont, Color::Black, Color(128, 128, 128), Transform({ 0, 0, 1 }));
		usernameInput.CreateText("Username:", displayFont, Color::Black, Transform({ -150, 30, 1 }), AlignH::Left, AlignV::Bottom);

		UITextInput& serverInput = usernameInput.CreateTextInput(300, 50, displayFont, Color::Black, Color(128, 128, 128), Transform({ 0, -75, 0 }));
		serverInput.CreateText("Server:", displayFont, Color::Black, Transform({ -150, 30, 1 }), AlignH::Left, AlignV::Bottom);
		serverInput.SetText("localhost:10000");

		UIRectangle& ec2Button = serverInput.CreateRectangle(50, 50, Color::Red, Transform({ 180, 0, 0 }));
		ec2Button.Events().OnClick().AddEventListener([&serverInput](Event<UI<MouseClickEvent>>& e)
			{
				serverInput.SetText("3.106.166.174:10000");
			});

		UIRectangle& connectButton = serverInput.CreateRectangle(300, 50, Color(50, 200, 50), Transform({ 0, -60, 0 }));
		connectButton.CreateText("Connect", displayFont, Color::Black, Transform({ 0, 0, 1 }));
		connectButton.Events().OnClick().AddEventListener([&background, &serverInput, &usernameInput, &gameScene, &scene](Event<UI<MouseClickEvent>>& e)
			{
				if (!ClientState::Get().HasConnection() || (!ClientState::Get().GetConnection().IsConnected() && !ClientState::Get().GetConnection().IsConnecting()))
				{
					ConnectToServerEvent data;
					data.Username = usernameInput.GetText();
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
					response.ContinueWithOnMainThread([&gameScene, &connectingIcon, username{ data.Username }](std::optional<ServerConnectionResponse> response)
						{
							connectingIcon.Remove();
							if (response)
							{
								SceneManager::Get().SetCurrentScene(gameScene, username);
							}
							else
							{
								BLT_WARN("Connection Failed");
								ClientState::Get().CloseConnection();
							}
						});
				}
			});
	}

	void CreateGameScene(Scene& scene, const Window& window)
	{
		scene.OnLoad().AddEventListener([&scene](Event<SceneLoadEvent>& e)
			{
				CreateCharacterRequest request;
				request.Name = std::any_cast<std::string>(e.Data.Data);
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

					EntityHandle player = entities.CreateFromEntityState(character->Data);
					ComponentHandle controller = player.Assign<CPlayerController>();
					controller->Speed = 10.0f;

					ActionHistory& actions = ClientState::Get().GetActionHistory();
					ActionRegistry& registry = ClientState::Get().GetActionRegistry();
					auto movementSystem = gameLayer.Systems().Add<MovementSystem>();
					auto controlSystem = gameLayer.Systems().Add<PlayerControlSystem>(&actions, &registry);
					auto indicatorSystem = gameLayer.Systems().Add<TileIndicatorSystem>();

					EntityFactory factory = gameLayer.GetFactory();
					EntityHandle tileIndicator = factory.Rectangle(1.0f, 1.0f, Color(255, 0, 0, 100));
					tileIndicator.Assign<CTileIndicator>();
					tileIndicator.Assign<CTilePosition>();

					entities.SetCamera(camera);
					entities.SetTileIndicator(tileIndicator);
					entities.SetControlledEntity(character->Data.NetworkId);

					std::optional<GetEntitiesResponse> otherEntities = ClientState::Get().GetConnection().GetSocketApi().GetEntities({ 0 }, 5.0).Result();
					if (otherEntities)
					{
						for (const EntityState& data : otherEntities->Entities)
						{
							if (data.NetworkId != character->Data.NetworkId)
							{
								entities.CreateFromEntityState(data);
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