#include "serverpch.h"
#include "ServerListener.h"
#include "ServerState.h"

#include "Lib/Entities/Components/NetworkId.h"

namespace Anarchy
{

#define ANCH_SERVER_BIND_FN(fn) std::bind(&fn, this, std::placeholders::_1)

	ServerListener::ServerListener(ServerSocket& socket)
		: m_Listener(), m_ServerSocket(socket), m_MessageHandlers()
	{
		Register<ServerConnectionResponse, ServerConnectionRequest>(ANCH_SERVER_BIND_FN(ServerListener::Connect));
		Register<ServerDisconnectResponse, ServerDisconnectRequest>(ANCH_SERVER_BIND_FN(ServerListener::Disconnect));
		Register<CreateCharacterResponse, CreateCharacterRequest>(ANCH_SERVER_BIND_FN(ServerListener::CreateCharacter));
		Register<GetEntitiesResponse, GetEntitiesRequest>(ANCH_SERVER_BIND_FN(ServerListener::GetEntities));

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

	std::optional<ServerConnectionResponse> ServerListener::Connect(const ServerRequest<ServerNetworkMessage<ServerConnectionRequest>>& request)
	{
		BLT_INFO("[SequenceId={0}] Connection Request Received", request.Request.SequenceId);
		if (request.Request.SequenceId == 0 && request.Request.ConnectionId == InvalidConnectionId)
		{
			ClientConnection& connection = ServerState::Get().GetConnections().AddConnection(request.Request.Message.Username, request.From);
			connection.SetSequenceId(request.Request.SequenceId);
			BLT_INFO("[ConnectionId={0}] [SequenceId={1}] Sending Connection Response", connection.GetConnectionId(), connection.GetSequenceId());
			ServerConnectionResponse response;
			response.ConnectionId = connection.GetConnectionId();
			return response;
		}
		return {};
	}

	std::optional<ServerDisconnectResponse> ServerListener::Disconnect(const ServerRequest<ServerNetworkMessage<ServerDisconnectRequest>>& request)
	{
		BLT_WARN("[ConnectionId={0}] [SequenceId={1}] Disconnect Request Received", request.Request.ConnectionId, request.Request.SequenceId);
		if (ServerState::Get().GetConnections().RemoveConnection(request.Request.ConnectionId))
		{
			DestroyEntitiesRequest destroyRequest;
			destroyRequest.Entities = ServerState::Get().GetEntities().GetAllIdsOwnedBy(request.Request.ConnectionId);
			DestroyEntities(ServerState::Get().GetConnections().GetConnectionIdsExcept(request.Request.ConnectionId), destroyRequest);
			BLT_INFO("[ConnectionId={0}] Sending Disconnect Response", request.Request.ConnectionId);
			ServerDisconnectResponse response;
			return response;
		}
		return {};
	}

	std::optional<CreateCharacterResponse> ServerListener::CreateCharacter(const ServerRequest<ServerNetworkMessage<CreateCharacterRequest>>& request)
	{
		BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Create Character Request Received", request.Request.ConnectionId, request.Request.SequenceId);
		ClientConnection* connection = GetConnection(request.Request.ConnectionId);
		if (connection != nullptr)
		{
			CreateCharacterResponse response;
			response.Data.NetworkId = ServerState::Get().GetEntities().GetNextEntityId();
			response.Data.Level = 1;
			response.Data.PrefabId = 0;
			response.Data.DimensionId = 0;
			response.Data.TilePosition = { Random::NextInt(0, 32), Random::NextInt(0, 18) };

			SpawnEntitiesRequest spawnRequest;
			spawnRequest.Entities.push_back(response.Data);
			SpawnEntities(ServerState::Get().GetConnections().GetConnectionIdsExcept(request.Request.ConnectionId), spawnRequest, request.Request.ConnectionId);

			connection->SetSequenceId(request.Request.SequenceId);
			BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Sending Create Character Response", request.Request.ConnectionId, connection->GetSequenceId());
			return response;
		}
		return {};
	}

	std::optional<GetEntitiesResponse> ServerListener::GetEntities(const ServerRequest<ServerNetworkMessage<GetEntitiesRequest>>& request)
	{
		BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Get Entities Request Received", request.Request.ConnectionId, request.Request.SequenceId);
		ClientConnection* connection = GetConnection(request.Request.ConnectionId);
		if (connection != nullptr)
		{
			connection->SetSequenceId(request.Request.SequenceId);
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
		for (const EntityData& entity : request.Entities)
		{
			EntityHandle e = ServerState::Get().GetEntities().CreateFromEntityData(entity, ownerConnectionId);
		}
		for (ClientConnection* connection : ServerState::Get().GetConnections().GetConnections(connections))
		{
			connection->SetSequenceId(connection->GetSequenceId());
			BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Sending Spawn Entities Request", connection->GetConnectionId(), connection->GetSequenceId());
			GetSocket().SendPacket(connection->GetAddress(), SpawnEntitiesRequest::Type, CreateMessage(connection->GetConnectionId(), request));
		}
	}

	void ServerListener::DestroyEntities(const std::vector<connid_t>& connections, const DestroyEntitiesRequest& request)
	{
		for (entityid_t id : request.Entities)
		{
			ServerState::Get().GetEntities().RemoveEntity(id);
		}
		for (ClientConnection* connection : ServerState::Get().GetConnections().GetConnections(connections))
		{
			connection->SetSequenceId(connection->GetSequenceId());
			BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Sending Destroy Entities Request", connection->GetConnectionId(), connection->GetSequenceId());
			GetSocket().SendPacket(connection->GetAddress(), DestroyEntitiesRequest::Type, CreateMessage(connection->GetConnectionId(), request));
		}
	}

	void ServerListener::UpdateEntities(const std::vector<connid_t>& connections, const UpdateEntitiesRequest& request)
	{
		for (ClientConnection* connection : ServerState::Get().GetConnections().GetConnections(connections))
		{
			connection->SetSequenceId(connection->GetSequenceId());
			BLT_TRACE("[ConnectionId={0}] [SequenceId={1}] Sending Update Entities Request", connection->GetConnectionId(), connection->GetSequenceId());
			GetSocket().SendPacket(connection->GetAddress(), UpdateEntitiesRequest::Type, CreateMessage(connection->GetConnectionId(), request));
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

}
