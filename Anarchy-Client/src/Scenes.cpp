#include "clientpch.h"
#include "Scenes.h"
#include "Events.h"

#include "ClientState.h"

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
		connectButton.Events().OnClick().AddEventListener([&background, &serverInput, &gameScene](Event<UI<MouseClickEvent>>& e)
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

					ServerConnectionRequest request;
					request.Username = data.Username;

					UIRectangle& connectingIcon = background.CreateRectangle(30, 30, Color::Red, Transform({ 170, -60, 1 }));

					Task<std::optional<ServerConnectionResponse>> response = ClientState::Get().GetConnection().Connect(request, 5.0);
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
				std::optional<CreateCharacterResponse> character = ClientState::Get().GetConnection().CreateCharacter(request, 5.0).Result();
				if (character)
				{
					Layer& layer = scene.AddLayer();
					EntityHandle camera = layer.GetFactory().Camera(Matrix4f::Orthographic(0, 32, 0, 18, -100, 100));
					layer.SetActiveCamera(camera);

					BLT_INFO("Created Character Successfully");
					ClientState::Get().InitializeEntities(scene, layer);
					EntityHandle player = ClientState::Get().GetEntities().CreateFromEntityData(character->Data);

					std::optional<GetEntitiesResponse> entities = ClientState::Get().GetConnection().GetEntities({ 0 }, 5.0).Result();
					if (entities)
					{
						for (const EntityData& data : entities->Entities)
						{
							if (data.NetworkId != character->Data.NetworkId)
							{
								ClientState::Get().GetEntities().CreateFromEntityData(data);
							}
						}
					}
				}
			});
		scene.OnUnload().AddEventListener([&scene](Event<SceneUnloadEvent>& e)
			{
				scene.Clear();
			});
	}

}