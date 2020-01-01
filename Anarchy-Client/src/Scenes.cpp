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
		connectButton.Events().OnClick().AddEventListener([&serverInput, &gameScene](Event<UI<MouseClickEvent>>& e)
			{
				ConnectToServerEvent data;
				data.Username = "Totomosic";
				data.Server = serverInput.GetText();
				//EventManager::Get().Bus().Emit(ClientEvents::ConnectToServer, data);

				size_t colon = data.Server.find(':');
				blt::string host = data.Server.substr(0, colon);
				blt::string port = data.Server.substr(colon + 1);
				SocketAddress address(host, port);
				ConnectionManager::Get().Initialize(address);

				ServerConnectionRequest request;
				request.Username = data.Username;

				Task<ServerConnectionResponse> response = ConnectionManager::Get().GetConnection().Connect(request);
				response.ContinueWithOnMainThread([&gameScene](ServerConnectionResponse response)
					{
						if (response.Success)
						{
							ConnectionManager::Get().SetConnectionId(response.ConnectionId);
							SceneManager::Get().SetCurrentScene(gameScene);
						}
						else
						{
							BLT_WARN("Connection Failed");
						}
					});
			});

		background.CreateText("Connecting As: Totomosic", displayFont, Color::Black, Transform({ 0, -100, 1 }));
	}

	void CreateGameScene(Scene& scene, const Window& window)
	{
	}

}