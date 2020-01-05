#include "clientpch.h"
#include "Client.h"

#include "Scenes.h"
#include "ClientState.h"

namespace Anarchy
{

	void Client::Init()
	{
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
				ClientState::Get().GetConnection().Disconnect({ ClientState::Get().GetConnection().GetConnectionId() }, 5.0).Wait();
			}
			ClientState::Get().CloseConnection();
		}
		ClientState::Terminate();
		Application::Exit();
	}

}