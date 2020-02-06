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
		: m_Connection(), m_Tilemap(), m_TilemapRenderer(), m_Entities()
	{
	}

	void ClientState::InitializeConnection(const SocketAddress& serverAddress)
	{
		m_Connection = std::make_unique<ConnectionManager>(serverAddress);
	}

	void ClientState::InitializeTilemap(Scene& gameScene, Layer& mapLayer, int width, int height)
	{
		m_Tilemap = std::make_unique<Tilemap>(width, height);
		m_TilemapRenderer = std::make_unique<TilemapRenderer>(&mapLayer, m_Tilemap.get(), 1.0f, 1.0f);
	}

	void ClientState::InitializeEntities(Scene& gameScene, Layer& gameLayer)
	{
		m_Entities = std::make_unique<ClientEntityCollection>(gameScene, gameLayer);
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

	const TilemapRenderer& ClientState::GetTilemapRenderer() const
	{
		return *m_TilemapRenderer;
	}

	TilemapRenderer& ClientState::GetTilemapRenderer()
	{
		return *m_TilemapRenderer;
	}

	void ClientState::DestroyTilemap()
	{
		m_TilemapRenderer = nullptr;
		m_Tilemap = nullptr;
	}

}