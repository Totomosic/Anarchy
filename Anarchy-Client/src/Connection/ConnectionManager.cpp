#include "clientpch.h"
#include "ConnectionManager.h"

#include "ClientState.h"

namespace Anarchy
{

#define ANCH_BIND_LISTENER_FN(func) std::bind(&func, this, std::placeholders::_1)

	ConnectionManager::ConnectionManager(const SocketAddress& address)
		: m_Bus(), m_TaskManager(m_Bus), m_Connection(address), m_ConnectionId(InvalidConnectionId), m_Connecting(false), m_SequenceId(0), m_Listener(), m_MessageHandlers()
	{
		Register<SpawnEntitiesRequest>(ANCH_BIND_LISTENER_FN(ConnectionManager::SpawnEntities));
		Register<DestroyEntitiesRequest>(ANCH_BIND_LISTENER_FN(ConnectionManager::DestroyEntities));
		Register<UpdateEntitiesRequest>(ANCH_BIND_LISTENER_FN(ConnectionManager::UpdateEntities));

		m_Listener = GetServerSocket().OnMessageReceived().AddScopedEventListener([this](Event<ServerMessageReceived>& e)
			{
				MessageType type = e.Data.Type;
				auto it = m_MessageHandlers.find(type);
				if (it != m_MessageHandlers.end())
				{
					it->second(e.Data.Data);
					e.StopPropagation();
				}
			});
	}

	ConnectionManager::~ConnectionManager()
	{
		m_Bus.Flush();
	}

	bool ConnectionManager::IsConnecting() const
	{
		return m_Connecting;
	}

	bool ConnectionManager::IsConnected() const
	{
		return m_ConnectionId != InvalidConnectionId;
	}

	const ServerConnection& ConnectionManager::GetServerSocket() const
	{
		return m_Connection;
	}

	ServerConnection& ConnectionManager::GetServerSocket()
	{
		return m_Connection;
	}

	connid_t ConnectionManager::GetConnectionId() const
	{
		return m_ConnectionId;
	}

	ClientSocketApi::Promise<ServerConnectionResponse> ConnectionManager::Connect(const ServerConnectionRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(!IsConnected() && !IsConnecting(), "Cannot connect now");
		m_Connecting = true;
		m_SequenceId = 0;
		return TaskManager::Get().Run([this, request, timeoutSeconds]()
			{
				auto response = AwaitResponse<ServerConnectionResponse>(CreateMessage(request), timeoutSeconds);
				m_Connecting = false;
				if (response)
				{
					m_ConnectionId = response->ConnectionId;
				}
				return response;
			});
	}

	ClientSocketApi::Promise<ServerDisconnectResponse> ConnectionManager::Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected() && !IsConnecting(), "Cannot disconnect now");
		return TaskManager::Get().Run([this, request, timeoutSeconds]()
			{
				m_Bus.Flush();
				auto response = AwaitResponse<ServerDisconnectResponse>(CreateMessage(request), timeoutSeconds);
				if (response)
				{
					m_Bus.Flush();
					m_ConnectionId = InvalidConnectionId;
				}
				return response;
			});
	}

	ClientSocketApi::Promise<CreateCharacterResponse> ConnectionManager::CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected(), "Must be connected");
		return TaskManager::Get().Run([this, request, timeoutSeconds]()
			{
				return AwaitResponse<CreateCharacterResponse>(CreateMessage(request), timeoutSeconds);
			});
	}

	ClientSocketApi::Promise<GetEntitiesResponse> ConnectionManager::GetEntities(const GetEntitiesRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected(), "Must be connected");
		return TaskManager::Get().Run([this, request, timeoutSeconds]()
			{
				return AwaitResponse<GetEntitiesResponse>(CreateMessage(request), timeoutSeconds);
			});
	}

	void ConnectionManager::SpawnEntities(const NetworkMessage<SpawnEntitiesRequest>& request)
	{
		for (const EntityData& entity : request.Message.Entities)
		{
			ClientState::Get().GetEntities().CreateFromEntityData(entity);
		}
	}

	void ConnectionManager::DestroyEntities(const NetworkMessage<DestroyEntitiesRequest>& request)
	{
		for (entityid_t entity : request.Message.Entities)
		{
			ClientState::Get().GetEntities().RemoveEntity(entity);
		}
	}

	void ConnectionManager::UpdateEntities(const NetworkMessage<UpdateEntitiesRequest>& request)
	{
		BLT_INFO("Updating...");
	}

}