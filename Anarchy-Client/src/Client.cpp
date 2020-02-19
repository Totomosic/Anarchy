#include "clientpch.h"
#include "Client.h"

#include "Scenes.h"
#include "ClientState.h"

namespace Anarchy
{

	Renderer2D* renderer;

	void Client::Init()
	{
		GetWindow().DisableVSync();
		Scene& titleScene = SceneManager::Get().AddScene();
		Scene& gameScene = SceneManager::Get().AddScene();

		CreateTitleScene(titleScene, GetWindow(), gameScene);
		CreateGameScene(gameScene, GetWindow());
		SceneManager::Get().SetCurrentScene(titleScene);

		renderer = new Renderer2D;
	}

	void Client::Tick()
	{
		
	}

	void Client::Update()
	{
		if (ClientState::Get().HasConnection())
		{
			ClientState::Get().GetConnection().Update(Time::Get().RenderingTimeline().DeltaTime());
			if (ClientState::Get().GetConnection().IsConnected())
			{
				if (Input::Get().KeyPressed(Keycode::Esc))
				{
					ClientState::Get().GetConnection().GetSocketApi().Disconnect({}, 2.0).ContinueWithOnMainThread([](std::optional<ServerDisconnectResponse> response)
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