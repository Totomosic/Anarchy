#include "clientpch.h"
#include "ConnectionManager.h"

#include "ClientState.h"
#include "Events.h"

namespace Anarchy
{

#define ANCH_BIND_LISTENER_FN(func) std::bind(&func, this, std::placeholders::_1)

	ConnectionManager::ConnectionManager(const SocketAddress& address)
		: m_Bus(), m_TaskManager(m_Bus), m_OnDisconnect(m_Bus.GetEmitter<ServerDisconnect>(ClientEvents::DisconnectedFromServer)), 
		m_Connection(address), m_ConnectionId(InvalidConnectionId), m_Connecting(false), m_SequenceId(0), m_Listener(), m_MessageHandlers(), m_TimeSinceKeepAlive(0), m_TimeSinceLastReceivedMessage(0)
	{
		Register<KeepAlivePacket>(ANCH_BIND_LISTENER_FN(ConnectionManager::OnKeepAlive));

		Register<SpawnEntitiesRequest>(ANCH_BIND_LISTENER_FN(ConnectionManager::SpawnEntities));
		Register<DestroyEntitiesRequest>(ANCH_BIND_LISTENER_FN(ConnectionManager::DestroyEntities));
		Register<UpdateEntitiesRequest>(ANCH_BIND_LISTENER_FN(ConnectionManager::UpdateEntities));

		Register<ForceDisconnectMessage>(ANCH_BIND_LISTENER_FN(ConnectionManager::OnForceDisconnect));

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

	seqid_t ConnectionManager::GetSequenceId() const
	{
		return m_SequenceId;
	}

	seqid_t ConnectionManager::GetRemoteSequenceId() const
	{
		return m_RemoteSequenceId;
	}

	EventEmitter<ServerDisconnect>& ConnectionManager::OnDisconnect()
	{
		return m_OnDisconnect;
	}

	void ConnectionManager::Update(TimeDelta delta)
	{
		if (IsConnected() && !IsConnecting())
		{
			m_TimeSinceLastReceivedMessage += delta.Milliseconds();
			if (m_TimeSinceLastReceivedMessage >= 5000)
			{
				std::optional<ServerDisconnectResponse> response = Disconnect({}, 2.0).Result();
				if (!response)
				{
					DisconnectInternal();
				}
			}
			else
			{
				m_TimeSinceKeepAlive += delta.Milliseconds();
				if (m_TimeSinceKeepAlive >= 1000)
				{
					SendKeepAlive();
					m_TimeSinceKeepAlive = 0;
				}
			}
		}
	}

	void ConnectionManager::SendKeepAlive()
	{
		BLT_ASSERT(IsConnected(), "Must be connected to send keep alive");
		KeepAlivePacket packet;
		GetServerSocket().SendPacket(MessageType::KeepAlive, CreateMessage(packet));
	}

	void ConnectionManager::OnKeepAlive(const NetworkMessage<KeepAlivePacket>& message)
	{
		ResetTimeSinceLastReceivedMessage();
	}

	void ConnectionManager::OnForceDisconnect(const NetworkMessage<ForceDisconnectMessage>& message)
	{
		if (IsSeqIdGreater(message.SequenceId, GetRemoteSequenceId()))
		{
			DisconnectInternal();
		}
	}

	ClientSocketApi::Promise<ServerConnectionResponse> ConnectionManager::Connect(const ServerConnectionRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(!IsConnected() && !IsConnecting(), "Cannot connect now");
		m_Connecting = true;
		m_SequenceId = 0;
		return TaskManager::Get().Run([this, request, timeoutSeconds]()
			{
				IncrementSequenceId();
				auto response = AwaitResponse<ServerConnectionResponse>(CreateMessage(request), timeoutSeconds);
				m_Connecting = false;
				if (response && IsSeqIdGreater(response->SequenceId, GetRemoteSequenceId()))
				{
					ResetTimeSinceLastReceivedMessage();
					m_ConnectionId = response->Message.ConnectionId;
					SetRemoteSequenceId(response->SequenceId);
					return MakeOptional(response);
				}
				return std::optional<ServerConnectionResponse>();
			});
	}

	ClientSocketApi::Promise<ServerDisconnectResponse> ConnectionManager::Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected() && !IsConnecting(), "Cannot disconnect now");
		return TaskManager::Get().Run([this, request, timeoutSeconds]()
			{
				m_Bus.Flush();
				IncrementSequenceId();
				auto response = AwaitResponse<ServerDisconnectResponse>(CreateMessage(request), timeoutSeconds);
				if (response && response->SequenceId == 0)
				{
					ResetTimeSinceLastReceivedMessage();
					SetRemoteSequenceId(response->SequenceId);
					DisconnectInternal();
					return MakeOptional(response);
				}
				return std::optional<ServerDisconnectResponse>();
			});
	}

	ClientSocketApi::Promise<CreateCharacterResponse> ConnectionManager::CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected(), "Must be connected");
		return TaskManager::Get().Run([this, request, timeoutSeconds]()
			{
				IncrementSequenceId();
				auto response = AwaitResponse<CreateCharacterResponse>(CreateMessage(request), timeoutSeconds);
				if (response && IsSeqIdGreater(response->SequenceId, GetRemoteSequenceId()))
				{
					ResetTimeSinceLastReceivedMessage();
					SetRemoteSequenceId(response->SequenceId);
					return MakeOptional(response);					
				}
				return std::optional<CreateCharacterResponse>();
			});
	}

	ClientSocketApi::Promise<GetEntitiesResponse> ConnectionManager::GetEntities(const GetEntitiesRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected(), "Must be connected");
		return TaskManager::Get().Run([this, request, timeoutSeconds]()
			{
				IncrementSequenceId();
				auto response = AwaitResponse<GetEntitiesResponse>(CreateMessage(request), timeoutSeconds);
				if (response && IsSeqIdGreater(response->SequenceId, GetRemoteSequenceId()))
				{
					ResetTimeSinceLastReceivedMessage();
					SetRemoteSequenceId(response->SequenceId);
					return MakeOptional(response);
				}
				return std::optional<GetEntitiesResponse>();
			});
	}

	void ConnectionManager::SpawnEntities(const NetworkMessage<SpawnEntitiesRequest>& request)
	{
		if (IsSeqIdGreater(request.SequenceId, GetRemoteSequenceId()))
		{
			ResetTimeSinceLastReceivedMessage();
			SetRemoteSequenceId(request.SequenceId);
			for (const EntityData& entity : request.Message.Entities)
			{
				ClientState::Get().GetEntities().CreateFromEntityData(entity);
			}
		}
	}

	void ConnectionManager::DestroyEntities(const NetworkMessage<DestroyEntitiesRequest>& request)
	{
		if (IsSeqIdGreater(request.SequenceId, GetRemoteSequenceId()))
		{
			ResetTimeSinceLastReceivedMessage();
			SetRemoteSequenceId(request.SequenceId);
			for (entityid_t entity : request.Message.Entities)
			{
				ClientState::Get().GetEntities().RemoveEntity(entity);
			}
		}
	}

	void ConnectionManager::UpdateEntities(const NetworkMessage<UpdateEntitiesRequest>& request)
	{
		if (IsSeqIdGreater(request.SequenceId, GetRemoteSequenceId()))
		{
			ResetTimeSinceLastReceivedMessage();
			SetRemoteSequenceId(request.SequenceId);
			BLT_INFO("Updating...");
		}
	}

	void ConnectionManager::SendMoveCommand(const EntityCommand<TileMovement>& command)
	{
	}

	void ConnectionManager::DisconnectInternal()
	{
		m_Bus.Flush();
		m_ConnectionId = InvalidConnectionId;
		OnDisconnect().Emit({});
	}

	void ConnectionManager::IncrementSequenceId(seqid_t amount)
	{
		m_SequenceId += amount;
	}

	void ConnectionManager::SetRemoteSequenceId(seqid_t seqId)
	{
		m_RemoteSequenceId = seqId;
	}

	void ConnectionManager::ResetTimeSinceLastReceivedMessage()
	{
		m_TimeSinceLastReceivedMessage = 0;
	}

}