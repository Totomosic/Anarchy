#include "clientpch.h"
#include "Scenes.h"
#include "Events.h"

#include "Connection/ConnectionManager.h"

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
				if (!ConnectionManager::Get().HasConnection())
				{
					ConnectToServerEvent data;
					data.Username = "Totomosic";
					data.Server = serverInput.GetText();
					//EventManager::Get().Bus().Emit(ClientEvents::ConnectToServer, data);

					size_t colon = data.Server.rfind(':');
					blt::string host = data.Server.substr(0, colon);
					blt::string port = data.Server.substr(colon + 1);
					SocketAddress address(host, port);
					ConnectionManager::Get().Initialize(address);

					ServerConnectionRequest request;
					request.Username = data.Username;

					UIRectangle& connectingIcon = background.CreateRectangle(30, 30, Color::Red, Transform({ 170, -60, 1 }));

					Task<std::optional<ServerConnectionResponse>> response = ConnectionManager::Get().Connect(request, 5.0);
					response.ContinueWithOnMainThread([&gameScene, &connectingIcon](std::optional<ServerConnectionResponse> response)
						{
							connectingIcon.Remove();
							if (response && response->Success)
							{
								ConnectionManager::Get().SetConnectionId(response->ConnectionId);
								SceneManager::Get().SetCurrentScene(gameScene);
								std::optional<CreateCharacterResponse> character = ConnectionManager::Get().CreateCharacter({ ConnectionManager::Get().GetConnectionId() }, 5.0).Result();
								if (character && character->Success)
								{
									BLT_TRACE("Successfully Connected to Server");
									BLT_TRACE("Connection Id {}", response->ConnectionId);
									BLT_TRACE("Character Id {}", character->Data.EntityId);
								}
								else
								{
									BLT_WARN("Failed to create character");
									ConnectionManager::Get().CloseConnection();
								}
							}
							else
							{
								BLT_WARN("Connection Failed");
								ConnectionManager::Get().CloseConnection();
							}
						});
				}
			});

		background.CreateText("Connecting As: Totomosic", displayFont, Color::Black, Transform({ 0, -100, 1 }));
	}

	void CreateGameScene(Scene& scene, const Window& window)
	{
	}

}