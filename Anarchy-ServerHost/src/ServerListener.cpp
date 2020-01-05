#include "serverpch.h"
#include "ServerListener.h"
#include "ServerState.h"

#include "Lib/Components/NetworkId.h"

namespace Anarchy
{

#define ANCH_SERVER_BIND_FN(fn) std::bind(&fn, this, std::placeholders::_1)

	ServerListener::ServerListener(ServerSocket& socket)
		: m_Listener(), m_ServerSocket(socket), m_MessageHandlers()
	{
		Register<ServerConnectionResponse, ServerConnectionRequest>(ANCH_SERVER_BIND_FN(ServerListener::Connect));
		Register<ServerDisconnectResponse, ServerDisconnectRequest>(ANCH_SERVER_BIND_FN(ServerListener::Disconnect));
		Register<CreateCharacterResponse, CreateCharacterRequest>(ANCH_SERVER_BIND_FN(ServerListener::CreateCharacter));

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
			EntityHandle entity = ServerState::Get().GetEntities().CreateEntity(0, request.Request.ConnectionId);

			response.Data.EntityId = entity.GetComponent<NetworkId>()->Id;
			response.Data.Name = ServerState::Get().GetConnections().GetConnection(request.Request.ConnectionId).GetUsername();
			response.Data.Level = 1;
			response.Data.PrefabId = 0;
			response.Data.DimensionId = 0;
			response.Data.TilePosition = { 0, 0 };

			response.Success = true;
		}
		return response;
	}

}