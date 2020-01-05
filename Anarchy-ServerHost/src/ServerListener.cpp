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

	ServerConnectionResponse ServerListener::Connect(const ServerRequest<ServerConnectionRequest>& request)
	{
		ClientConnection& connection = ServerState::Get().GetConnections().AddConnection(request.Request.Username, request.From);
		ServerConnectionResponse response;
		response.Success = true;
		response.ConnectionId = connection.GetConnectionId();
		BLT_INFO("Connection Request Received: Connecting as {0}, Id {1}", connection.GetUsername(), response.ConnectionId);
		return response;
	}

	ServerDisconnectResponse ServerListener::Disconnect(const ServerRequest<ServerDisconnectRequest>& request)
	{
		bool success = ServerState::Get().GetConnections().RemoveConnection(request.Request.ConnectionId);
		ServerState::Get().GetEntities().RemoveAllOwnedBy(request.Request.ConnectionId);
		ServerDisconnectResponse response;
		response.Success = success;
		BLT_WARN("[{0}] Disconnect Request Received", request.Request.ConnectionId);
		return response;
	}

	CreateCharacterResponse ServerListener::CreateCharacter(const ServerRequest<CreateCharacterRequest>& request)
	{
		CreateCharacterResponse response;
		response.Success = false;
		BLT_TRACE("[{0}] Create Character Request Received", request.Request.ConnectionId);
		if (ServerState::Get().GetConnections().HasConnection(request.Request.ConnectionId))
		{
			response.Data.NetworkId = ServerState::Get().GetEntities().GetNextEntityId();
			response.Data.Level = 1;
			response.Data.PrefabId = 0;
			response.Data.DimensionId = 0;
			response.Data.TilePosition = { Random::NextInt(0, 16), Random::NextInt(0, 9) };

			EntityHandle entity = ServerState::Get().GetEntities().CreateFromEntityData(response.Data, request.Request.ConnectionId);

			response.Success = true;
		}
		return response;
	}

	GetEntitiesResponse ServerListener::GetEntities(const ServerRequest<GetEntitiesRequest>& request)
	{
		BLT_TRACE("[{0}] Get Entities Request Received", request.Request.ConnectionId);
		GetEntitiesResponse response;
		for (EntityHandle entity : ServerState::Get().GetEntities().GetAllEntities())
		{
			response.Entities.push_back(ServerState::Get().GetEntities().GetDataFromEntity(entity));
		}
		return response;
	}

}