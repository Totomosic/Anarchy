#include "clientpch.h"
#include "Client.h"

#include "Scenes.h"
#include "ClientState.h"

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
		if (ClientState::Get().HasConnection() && ClientState::Get().GetConnection().IsConnected())
		{
			ClientState::Get().GetConnection().Update(Time::Get().RenderingTimeline().DeltaTime());
			if (Input::Get().KeyPressed(Keycode::Esc))
			{
				Task t = ClientState::Get().GetConnection().GetSocketApi().Disconnect({}, 2.0);
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