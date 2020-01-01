#include "clientpch.h"
#include "Client.h"

#include "Scenes.h"
#include "Connection/ConnectionManager.h"

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
		ConnectionManager::Get().GetConnection().RequestDisconnect(ConnectionManager::Get().GetConnectionId());
		ConnectionManager::Get().CloseConnection();
		ConnectionManager::Terminate();
		Application::Exit();
	}

}