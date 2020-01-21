#include "serverpch.h"
#include "ServerListener.h"
#include "ServerState.h"

#include "Lib/Entities/Components/NetworkId.h"

namespace Anarchy
{

#define ANCH_SERVER_BIND_FN(fn) std::bind(&fn, this, std::placeholders::_1)

	ServerListener::ServerListener(ServerSocket& socket)
		: m_Listener(), m_ServerSocket(socket), m_MessageHandlers(), m_CommandBuffer(), m_Mutex()
	{
		Register<ServerConnectionResponse, ServerConnectionRequest>(ANCH_SERVER_BIND_FN(ServerListener::Connect));
		Register<ServerDisconnectResponse, ServerDisconnectRequest>(ANCH_SERVER_BIND_FN(ServerListener::Disconnect));
		Register<CreateCharacterResponse, CreateCharacterRequest>(ANCH_SERVER_BIND_FN(ServerListener::CreateCharacter));
		Register<GetEntitiesResponse, GetEntitiesRequest>(ANCH_SERVER_BIND_FN(ServerListener::GetEntities));
		Register<GenericCommand>(ANCH_SERVER_BIND_FN(ServerListener::OnCommand));

		Register<KeepAlivePacket>(ANCH_SERVER_BIND_FN(ServerListener::OnKeepAlive));

		m_Listener = m_ServerSocket.OnMessageReceived().AddScopedEventListener([this](Event<ClientMessageReceived>& e)
			{
				MessageType type = e.Data.Type;
				auto it = m_MessageHandlers.find(type);
				if (it != m_MessageHandlers.end())
				{
					it->second(e.Data.From, e.Data.Data);
					e.StopPropagation();
				}
			});
	}

	CommandBuffer* ServerListener::GetCommandBuffer() const
	{
		return m_CommandBuffer;
	}

	void ServerListener::SetCommandBuffer(CommandBuffer* buffer)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		m_CommandBuffer = buffer;
	}

	void ServerListener::Update(TimeDelta delta)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		std::vector<connid_t> connectionsToRemove;
		std::vector<connid_t> keepAliveConnections;
		for (ClientConnection* connection : ServerState::Get().GetConnections().GetConnections())
		{
			connection->UpdateTimeSinceLastPacket(delta);
			connection->UpdateTimeSinceLastSentPacket(delta);
			if (connection->GetTimeSinceLastPacket() >= 5000)
			{
				connectionsToRemove.push_back(connection->GetConnectionId());
			}
			else if (connection->GetTimeSinceLastSentPacket() >= 500)
			{
				keepAliveConnections.push_back(connection->GetConnectionId());
			}
		}
		ForceDisconnectConnectionsInternal(connectionsToRemove);
		SendKeepAliveInternal(keepAliveConnections);
	}

	void ServerListener::OnKeepAlive(const ServerNetworkMessage<KeepAlivePacket>& packet)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		ClientConnection* connection = GetConnection(packet.ConnectionId);
		if (connection != nullptr)
		{
			HandleIncomingMessage(packet, false);
			connection->ResetTimeSinceLastPacket();
		}
	}

	void ServerListener::SendKeepAlive(const std::vector<connid_t>& connections)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		SendKeepAliveInternal(connections);
	}

	void ServerListener::ForceDisconnectConnections(const std::vector<connid_t>& connectionIds)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		ForceDisconnectConnectionsInternal(connectionIds);
	}

	std::optional<ServerConnectionResponse> ServerListener::Connect(const ServerRequest<ServerNetworkMessage<ServerConnectionRequest>>& request)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		BLT_INFO("[SequenceId={0}] Connection Request Received", request.Request.Header.SequenceId);
		if (IsSeqIdGreater(request.Request.Header.SequenceId, 0) && request.Request.ConnectionId == InvalidConnectionId)
		{
			HandleIncomingMessage(request.Request);
			ClientConnection& connection = ServerState::Get().GetConnections().AddConnection(request.Request.Message.Username, request.From);
			connection.SetRemoteSequenceId(request.Request.Header.SequenceId);
			connection.SetSequenceId(1);
			connection.ResetTimeSinceLastPacket();
			connection.ResetTimeSinceLastSentPacket();
			BLT_INFO("[ConnectionId={0}] [SequenceId={1}] Sending Connection Response", connection.GetConnectionId(), connection.GetSequenceId());
			((ServerRequest<ServerNetworkMessage<ServerConnectionRequest>>&)request).Request.ConnectionId = connection.GetConnectionId();
			ServerConnectionResponse response;
			response.ConnectionId = connection.GetConnectionId();
			return response;
		}
		return {};
	}

	std::optional<ServerDisconnectResponse> ServerListener::Disconnect(const ServerRequest<ServerNetworkMessage<ServerDisconnectRequest>>& request)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		BLT_WARN("[ConnectionId={0}] [SequenceId={1}] Disconnect Request Received", request.Request.ConnectionId, request.Request.Header.SequenceId);
		ClientConnection* connection = GetConnection(request.Request.ConnectionId);
		if (connection != nullptr)
		{
			if (IsSeqIdGreater(request.Request.Header.SequenceId, connection->GetRemoteSequenceId()) && ServerState::Get().GetConnections().RemoveConnection(request.Request.ConnectionId))
			{
				HandleIncomingMessage(request.Request);
				connection->SetRemoteSequenceId(request.Request.Header.SequenceId);
				connection->IncrementSequenceId();
				CleanupConnection(request.Request.ConnectionId);
				BLT_INFO("[ConnectionId={0}] [SequenceId={1}] Sending Disconnect Response", request.Request.ConnectionId, GetSequenceId(request.Request.ConnectionId));
				ServerDisconnectResponse response;
				return response;
			}
		}
		return {};
	}

	std::optional<CreateCharacterResponse> ServerListener::CreateCharacter(const ServerRequest<ServerNetworkMessage<CreateCharacterRequest>>& request)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Create Character Request Received", request.Request.ConnectionId, request.Request.Header.SequenceId);
		ClientConnection* connection = GetConnection(request.Request.ConnectionId);
		if (connection != nullptr && IsSeqIdGreater(request.Request.Header.SequenceId, connection->GetRemoteSequenceId()))
		{
			HandleIncomingMessage(request.Request);
			CreateCharacterResponse response;
			response.Data.NetworkId = ServerState::Get().GetEntities().GetNextEntityId();
			response.Data.Level = 1;
			response.Data.PrefabId = 0;
			response.Data.DimensionId = 0;
			response.Data.TilePosition = { Random::NextInt(0, 32), Random::NextInt(0, 18) };

			SpawnEntitiesRequest spawnRequest;
			spawnRequest.Entities.push_back(response.Data);
			SpawnEntitiesInternal(ServerState::Get().GetConnections().GetConnectionIdsExcept(request.Request.ConnectionId), spawnRequest, request.Request.ConnectionId);

			connection->SetRemoteSequenceId(request.Request.Header.SequenceId);
			connection->IncrementSequenceId();
			connection->ResetTimeSinceLastPacket();
			connection->ResetTimeSinceLastSentPacket();
			BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Sending Create Character Response", request.Request.ConnectionId, connection->GetSequenceId());
			return response;
		}
		return {};
	}

	std::optional<GetEntitiesResponse> ServerListener::GetEntities(const ServerRequest<ServerNetworkMessage<GetEntitiesRequest>>& request)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Get Entities Request Received", request.Request.ConnectionId, request.Request.Header.SequenceId);
		ClientConnection* connection = GetConnection(request.Request.ConnectionId);
		if (connection != nullptr && IsSeqIdGreater(request.Request.Header.SequenceId, connection->GetRemoteSequenceId()))
		{
			HandleIncomingMessage(request.Request);
			connection->SetRemoteSequenceId(request.Request.Header.SequenceId);
			connection->IncrementSequenceId();
			connection->ResetTimeSinceLastPacket();
			connection->ResetTimeSinceLastSentPacket();
			BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Sending Get Entities Response", request.Request.ConnectionId, connection->GetSequenceId());
			GetEntitiesResponse response;
			for (EntityHandle entity : ServerState::Get().GetEntities().GetAllEntities())
			{
				response.Entities.push_back(ServerState::Get().GetEntities().GetDataFromEntity(entity));
			}
			return response;
		}
		return {};
	}

	void ServerListener::SpawnEntities(const std::vector<connid_t>& connections, const SpawnEntitiesRequest& request, connid_t ownerConnectionId)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		SpawnEntitiesInternal(connections, request, ownerConnectionId);
	}

	void ServerListener::DestroyEntities(const std::vector<connid_t>& connections, const DestroyEntitiesRequest& request)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		DestroyEntitiesInternal(connections, request);
	}

	void ServerListener::UpdateEntities(const std::vector<connid_t>& connections, const UpdateEntitiesRequest& request)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		UpdateEntitiesInternal(connections, request);
	}

	void ServerListener::OnCommand(const ServerNetworkMessage<GenericCommand>& command)
	{
		std::scoped_lock<std::mutex> lock(m_Mutex);
		BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Command Received", command.ConnectionId, command.Header.SequenceId);
		ClientConnection* connection = GetConnection(command.ConnectionId);
		if (connection != nullptr)
		{
			HandleIncomingMessage(command);
			connection->SetRemoteSequenceId(command.Header.SequenceId);
			connection->ResetTimeSinceLastPacket();
			if (m_CommandBuffer != nullptr)
			{
				m_CommandBuffer->PushCommand(command.Message);
			}
		}
	}

	void ServerListener::SendKeepAliveInternal(const std::vector<connid_t>& connections)
	{
		for (connid_t connectionId : connections)
		{
			ClientConnection* connection = GetConnection(connectionId);
			if (connection != nullptr)
			{
				KeepAlivePacket packet;
				auto message = CreateMessage(connectionId, packet);
				HandleOutgoingMessage(connection, message, false);
				GetSocket().SendPacket(connection->GetAddress(), MessageType::KeepAlive, message);
			}
		}
	}

	void ServerListener::ForceDisconnectConnectionsInternal(const std::vector<connid_t>& connectionIds)
	{
		for (connid_t connectionId : connectionIds)
		{
			ClientConnection* connection = GetConnection(connectionId);
			if (connection != nullptr)
			{
				connection->IncrementSequenceId();
				BLT_ERROR("[ConnectionId={0}] [SequenceId={1}] Sending Force Disconnect Message", connectionId, connection->GetSequenceId());
				ForceDisconnectMessage message;
				GetSocket().SendPacket(connection->GetAddress(), MessageType::ForceDisconnect, CreateMessage(connectionId, message));
				CleanupConnection(connectionId);
				ServerState::Get().GetConnections().RemoveConnection(connectionId);
			}
		}
	}

	void ServerListener::SpawnEntitiesInternal(const std::vector<connid_t>& connections, const SpawnEntitiesRequest& request, connid_t ownerConnectionId)
	{
		for (const EntityData& entity : request.Entities)
		{
			EntityHandle e = ServerState::Get().GetEntities().CreateFromEntityData(entity, ownerConnectionId);
		}
		for (ClientConnection* connection : ServerState::Get().GetConnections().GetConnections(connections))
		{
			connection->IncrementSequenceId();
			connection->ResetTimeSinceLastSentPacket();
			BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Sending Spawn Entities Request", connection->GetConnectionId(), connection->GetSequenceId());
			auto message = CreateMessage(connection->GetConnectionId(), request);
			HandleOutgoingMessage(connection, message);
			GetSocket().SendPacket(connection->GetAddress(), SpawnEntitiesRequest::Type, message);
		}
	}

	void ServerListener::DestroyEntitiesInternal(const std::vector<connid_t>& connections, const DestroyEntitiesRequest& request)
	{
		for (entityid_t id : request.Entities)
		{
			ServerState::Get().GetEntities().RemoveEntity(id);
		}
		for (ClientConnection* connection : ServerState::Get().GetConnections().GetConnections(connections))
		{
			connection->IncrementSequenceId();
			connection->ResetTimeSinceLastSentPacket();
			BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Sending Destroy Entities Request", connection->GetConnectionId(), connection->GetSequenceId());
			auto message = CreateMessage(connection->GetConnectionId(), request);
			HandleOutgoingMessage(connection, message);
			GetSocket().SendPacket(connection->GetAddress(), DestroyEntitiesRequest::Type, message);
		}
	}

	void ServerListener::UpdateEntitiesInternal(const std::vector<connid_t>& connections, const UpdateEntitiesRequest& request)
	{
		for (ClientConnection* connection : ServerState::Get().GetConnections().GetConnections(connections))
		{
			connection->IncrementSequenceId();
			connection->ResetTimeSinceLastSentPacket();
			//BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Sending Update Entities Request", connection->GetConnectionId(), connection->GetSequenceId());
			auto message = CreateMessage(connection->GetConnectionId(), request);
			HandleOutgoingMessage(connection, message);
			GetSocket().SendPacket(connection->GetAddress(), UpdateEntitiesRequest::Type, message);
		}
	}

	void ServerListener::HandlePacketAcked(seqid_t sequenceId, PacketData* data, ClientConnection* connection) const
	{
		data->Acked = true;
		uint64_t timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(GetTimestamp() - data->Timestamp).count();
		if (connection != nullptr)
		{
			connection->UpdateAverageRTT(timeDelta);
		}
	}

	ClientConnection* ServerListener::GetConnection(connid_t connectionId) const
	{
		if (ServerState::Get().GetConnections().HasConnection(connectionId))
		{
			return &ServerState::Get().GetConnections().GetConnection(connectionId);
		}
		return nullptr;
	}

	seqid_t ServerListener::GetSequenceId(connid_t connectionId) const
	{
		ClientConnection* connection = GetConnection(connectionId);
		if (connection != nullptr)
		{
			return connection->GetSequenceId();
		}
		return 0;
	}

	ServerSocket& ServerListener::GetSocket() const
	{
		return ServerState::Get().GetSocket();
	}

	void ServerListener::CleanupConnection(connid_t connectionId)
	{
		DestroyEntitiesRequest destroyRequest;
		destroyRequest.Entities = ServerState::Get().GetEntities().GetAllIdsOwnedBy(connectionId);
		DestroyEntitiesInternal(ServerState::Get().GetConnections().GetConnectionIdsExcept(connectionId), destroyRequest);
	}

}
