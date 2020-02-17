#include "clientpch.h"
#include "ClientListener.h"

#include "ClientState.h"
#include "Events.h"

namespace Anarchy
{

#define ANCH_BIND_LISTENER_FN(func) std::bind(&func, this, std::placeholders::_1)

	ClientListener::ClientListener(ClientSocket* socket)
		: m_Bus(), m_TaskManager(m_Bus), m_OnDisconnect(m_Bus.GetEmitter<ServerDisconnect>(ClientEvents::DisconnectedFromServer)), m_ConnectionId(InvalidConnectionId), m_Connecting(false),
		m_Socket(socket), m_SequenceId(0), m_RemoteSequenceId(0), m_NextRequestId(0), m_Listener(), m_MessageHandlers(), m_RequestMutex(), m_RequestHandlers(),
		m_TimeSinceLastReceivedMessage(0), m_TimeSinceLastSentMessage(0), m_ReceivedMessages(), m_SentMessages(), m_Actions()
	{
		Register<KeepAlivePacket>(ANCH_BIND_LISTENER_FN(ClientListener::OnKeepAlive));

		Register<SpawnEntitiesRequest>(ANCH_BIND_LISTENER_FN(ClientListener::SpawnEntities));
		Register<DestroyEntitiesRequest>(ANCH_BIND_LISTENER_FN(ClientListener::DestroyEntities));
		Register<UpdateEntitiesRequest>(ANCH_BIND_LISTENER_FN(ClientListener::UpdateEntities));

		Register<ForceDisconnectMessage>(ANCH_BIND_LISTENER_FN(ClientListener::OnForceDisconnect));

		m_Listener = GetClientSocket().OnMessageReceived().AddScopedEventListener([this](Event<ServerMessageReceived>& e)
			{
				MessageType type = e.Data.Type;
				auto it = m_MessageHandlers.find(type);
				if (it != m_MessageHandlers.end())
				{
					it->second(e.Data.Data);
					e.StopPropagation();
				}
				else
				{
					MessageCategory	category = GetMessageCategory(type);
					if (category == MessageCategory::Response)
					{
						// All requests begin with a request header and all responses begin with a response header
						ResponseHeader header;
						Deserialize(e.Data.Data, header);
						// Find a response handler that corresponds to the request id
						auto it = m_RequestHandlers.find(header.RequestId);
						if (it != m_RequestHandlers.end())
						{
							it->second(e.Data.Data);
						}
					}
				}
			});
	}

	ClientListener::~ClientListener()
	{
		m_Bus.Flush();
	}

	const ClientSocket& ClientListener::GetClientSocket() const
	{
		return *m_Socket;
	}

	ClientSocket& ClientListener::GetClientSocket()
	{
		return *m_Socket;
	}

	connid_t ClientListener::GetConnectionId() const
	{
		return m_ConnectionId;
	}

	ActionBuffer& ClientListener::GetActionBuffer()
	{
		return m_Actions;
	}

	seqid_t ClientListener::GetSequenceId() const
	{
		return m_SequenceId;
	}

	seqid_t ClientListener::GetRemoteSequenceId() const
	{
		return m_RemoteSequenceId;
	}

	EventEmitter<ServerDisconnect>& ClientListener::OnDisconnect()
	{
		return m_OnDisconnect;
	}

	bool ClientListener::IsConnecting() const
	{
		return m_Connecting;
	}

	bool ClientListener::IsConnected() const
	{
		return m_ConnectionId != InvalidConnectionId;
	}

	void ClientListener::Update(TimeDelta delta)
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
				for (const GenericAction& action : m_Actions.GetNetworkActions())
				{
					SendAction(action);
				}
				m_Actions.ProcessAllActions();
				m_Actions.Clear();
				m_TimeSinceLastSentMessage += delta.Milliseconds();
				if (m_TimeSinceLastSentMessage >= 500)
				{
					SendKeepAlive();
					ResetTimeSinceLastSentMessage();
				}
			}
		}
		GetClientSocket().Update(delta);
	}

	void ClientListener::SendKeepAlive()
	{
		BLT_ASSERT(IsConnected(), "Must be connected to send keep alive");
		KeepAlivePacket packet;
		auto message = CreateMessage(packet);
		HandleOutgoingMessage(message, false);
		GetClientSocket().SendPacket(MessageType::KeepAlive, message);
	}

	void ClientListener::OnKeepAlive(const NetworkMessage<KeepAlivePacket>& message)
	{
		HandleIncomingMessage(message, false);
		ResetTimeSinceLastReceivedMessage();
	}

	void ClientListener::OnForceDisconnect(const NetworkMessage<ForceDisconnectMessage>& message)
	{
		if (IsSeqIdGreater(message.Header.SequenceId, GetRemoteSequenceId()))
		{
			HandleIncomingMessage(message);
			DisconnectInternal();
		}
	}

	ClientSocketApi::Promise<ServerConnectionResponse> ClientListener::Connect(const ServerConnectionRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(!IsConnected() && !IsConnecting(), "Cannot connect now");
		m_Connecting = true;
		m_SequenceId = 0;
		return TaskManager::Get().Run([this, requestCopy{ request }, timeoutSeconds]() mutable
			{
				IncrementSequenceId();
				ResetTimeSinceLastSentMessage();
				auto message = CreateMessage(requestCopy);
				HandleOutgoingMessage(message);
				auto response = AwaitResponse<ServerConnectionResponse>(message, timeoutSeconds);
				m_Connecting = false;
				if (response)
				{
					ResetTimeSinceLastReceivedMessage();
					m_ConnectionId = response->Message.ConnectionId;
					SetRemoteSequenceId(response->Header.SequenceId);
					return MakeOptional(response);
				}
				return std::optional<ServerConnectionResponse>();
			});
	}

	ClientSocketApi::Promise<ServerDisconnectResponse> ClientListener::Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected() && !IsConnecting(), "Cannot disconnect now");
		return TaskManager::Get().Run([this, requestCopy{ request }, timeoutSeconds]() mutable
			{
				m_Bus.Flush();
				IncrementSequenceId();
				ResetTimeSinceLastSentMessage();
				auto message = CreateMessage(requestCopy);
				HandleOutgoingMessage(message);
				auto response = AwaitResponse<ServerDisconnectResponse>(message, timeoutSeconds);
				if (response && response->Header.SequenceId == 0)
				{
					ResetTimeSinceLastReceivedMessage();
					SetRemoteSequenceId(response->Header.SequenceId);
					DisconnectInternal();
					return MakeOptional(response);
				}
				return std::optional<ServerDisconnectResponse>();
			});
	}

	ClientSocketApi::Promise<CreateCharacterResponse> ClientListener::CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected(), "Must be connected");
		return TaskManager::Get().Run([this, requestCopy{ request }, timeoutSeconds]() mutable
			{
				IncrementSequenceId();
				ResetTimeSinceLastSentMessage();
				auto message = CreateMessage(requestCopy);
				HandleOutgoingMessage(message);
				auto response = AwaitResponse<CreateCharacterResponse>(message, timeoutSeconds);
				if (response)
				{
					ResetTimeSinceLastReceivedMessage();
					SetRemoteSequenceId(response->Header.SequenceId);
					return MakeOptional(response);
				}
				return std::optional<CreateCharacterResponse>();
			});
	}

	ClientSocketApi::Promise<GetEntitiesResponse> ClientListener::GetEntities(const GetEntitiesRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected(), "Must be connected");
		return TaskManager::Get().Run([this, requestCopy{ request }, timeoutSeconds]() mutable
			{
				IncrementSequenceId();
				ResetTimeSinceLastSentMessage();
				auto message = CreateMessage(requestCopy);
				HandleOutgoingMessage(message);
				auto response = AwaitResponse<GetEntitiesResponse>(message, timeoutSeconds);
				if (response)
				{
					ResetTimeSinceLastReceivedMessage();
					SetRemoteSequenceId(response->Header.SequenceId);
					return MakeOptional(response);
				}
				return std::optional<GetEntitiesResponse>();
			});
	}

	ClientSocketApi::Promise<GetTilemapResponse> ClientListener::GetTilemap(const GetTilemapRequest& request, double timeoutSeconds)
	{
		BLT_ASSERT(IsConnected(), "Must be connected");
		return TaskManager::Get().Run([this, requestCopy{ request }, timeoutSeconds]() mutable
			{
				IncrementSequenceId();
				ResetTimeSinceLastSentMessage();
				auto message = CreateMessage(requestCopy);
				HandleOutgoingMessage(message);
				auto response = AwaitResponse<GetTilemapResponse>(message, timeoutSeconds);
				if (response)
				{
					ResetTimeSinceLastReceivedMessage();
					SetRemoteSequenceId(response->Header.SequenceId);
					return MakeOptional(response);
				}
				return std::optional<GetTilemapResponse>();
			});
	}

	void ClientListener::SpawnEntities(const NetworkMessage<SpawnEntitiesRequest>& request)
	{
		if (IsSeqIdGreater(request.Header.SequenceId, GetRemoteSequenceId()))
		{
			HandleIncomingMessage(request);
			ResetTimeSinceLastReceivedMessage();
			SetRemoteSequenceId(request.Header.SequenceId);
			for (const EntityData& entity : request.Message.Entities)
			{
				ClientState::Get().GetEntities().CreateFromEntityData(entity);
			}
		}
	}

	void ClientListener::DestroyEntities(const NetworkMessage<DestroyEntitiesRequest>& request)
	{
		HandleIncomingMessage(request);
		ResetTimeSinceLastReceivedMessage();
		SetRemoteSequenceId(request.Header.SequenceId);
		for (entityid_t entity : request.Message.Entities)
		{
			ClientState::Get().GetEntities().RemoveEntity(entity);
		}
	}

	void ClientListener::UpdateEntities(const NetworkMessage<UpdateEntitiesRequest>& request)
	{
		if (IsSeqIdGreater(request.Header.SequenceId, GetRemoteSequenceId()))
		{
			HandleIncomingMessage(request);
			ResetTimeSinceLastReceivedMessage();
			SetRemoteSequenceId(request.Header.SequenceId);
			if (IsConnected())
			{
				SendKeepAlive();
			}

			for (const GenericAction& action : request.Message.Updates)
			{
				GetActionBuffer().PushAction(action, false);
			}
		}
	}

	void ClientListener::SendAction(const GenericAction& action)
	{
		IncrementSequenceId();
		ResetTimeSinceLastSentMessage();
		auto message = CreateMessage(action);
		HandleOutgoingMessage(message);
		GetClientSocket().SendPacket(MessageType::InputCommand, message);
	}

	void ClientListener::DisconnectInternal()
	{
		m_Bus.Flush();
		m_ConnectionId = InvalidConnectionId;
		OnDisconnect().Emit({});
	}

	void ClientListener::IncrementSequenceId(seqid_t amount)
	{
		m_SequenceId += amount;
	}

	void ClientListener::SetRemoteSequenceId(seqid_t seqId)
	{
		if (IsSeqIdGreater(seqId, m_RemoteSequenceId))
		{
			m_RemoteSequenceId = seqId;
		}
	}

	void ClientListener::ResetTimeSinceLastReceivedMessage()
	{
		m_TimeSinceLastReceivedMessage = 0.0;
	}

	void ClientListener::ResetTimeSinceLastSentMessage()
	{
		m_TimeSinceLastSentMessage = 0.0;
	}

	void ClientListener::HandlePacketAcked(seqid_t sequenceId, PacketData* data) const
	{
		data->Acked = true;
		uint64_t deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(GetTimestamp() - data->Timestamp).count();
		BLT_INFO("Got Ack for SequenceId={0}, RTT={1}ms", sequenceId, deltaTime);
	}

}