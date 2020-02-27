#include "clientpch.h"
#include "ClientState.h"

namespace Anarchy
{

	std::unique_ptr<ClientState> ClientState::s_Instance;

	ClientState& ClientState::Get()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = std::make_unique<ClientState>();
		}
		return *s_Instance;
	}

	void ClientState::Terminate()
	{
		s_Instance = nullptr;
	}

	ClientState::ClientState()
		: m_Connection(), m_Tilemap(), m_Entities(), m_Actions(), m_ActionRegistry()
	{
	}

	void ClientState::InitializeConnection(const SocketAddress& serverAddress)
	{
		m_Connection = std::make_unique<ConnectionManager>(serverAddress);
	}

	void ClientState::InitializeTilemap(Scene& gameScene, Layer& mapLayer, int width, int height)
	{
		m_Tilemap = std::make_unique<Tilemap>(&mapLayer, width, height);
	}

	void ClientState::InitializeEntities(Scene& gameScene, Layer& gameLayer)
	{
		m_Entities = std::make_unique<ClientEntityCollection>(gameScene, gameLayer);
		m_Actions.Reset();
	}

	bool ClientState::HasConnection() const
	{
		return m_Connection != nullptr;
	}

	const ConnectionManager& ClientState::GetConnection() const
	{
		return *m_Connection;
	}

	ConnectionManager& ClientState::GetConnection()
	{
		return *m_Connection;
	}

	void ClientState::CloseConnection()
	{
		m_Connection = nullptr;
	}

	bool ClientState::HasEntities() const
	{
		return m_Entities != nullptr;
	}

	const ClientEntityCollection& ClientState::GetEntities() const
	{
		return *m_Entities;
	}

	ClientEntityCollection& ClientState::GetEntities()
	{
		return *m_Entities;
	}

	void ClientState::DestroyEntities()
	{
		m_Entities = nullptr;
	}

	bool ClientState::HasTilemap() const
	{
		return m_Tilemap != nullptr;
	}

	const Tilemap& ClientState::GetTilemap() const
	{
		return *m_Tilemap;
	}

	Tilemap& ClientState::GetTilemap()
	{
		return *m_Tilemap;
	}

	void ClientState::DestroyTilemap()
	{
		m_Tilemap = nullptr;
	}

	const ActionHistory& ClientState::GetActionHistory() const
	{
		return m_Actions;
	}

	ActionHistory& ClientState::GetActionHistory()
	{
		return m_Actions;
	}

	const ActionRegistry& ClientState::GetActionRegistry() const
	{
		return m_ActionRegistry;
	}

	ActionRegistry& ClientState::GetActionRegistry()
	{
		return m_ActionRegistry;
	}

}