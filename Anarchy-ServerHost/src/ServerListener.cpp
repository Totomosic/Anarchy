#include "serverpch.h"
#include "ServerListener.h"
#include "ServerState.h"

namespace Anarchy
{

	ServerListener::ServerListener(ServerSocket& socket)
		: m_Listener(), m_ServerSocket(socket)
	{
		m_Listener = m_ServerSocket.OnMessageReceived().AddScopedEventListener([this](Event<ClientMessageReceived>& e)
			{
				switch (e.Data.Type)
				{
					case MessageType::ConnectRequest:
					{
						ServerConnectionRequest request;
						Deserialize(e.Data.Data, request);
						OnConnectionRequest({ e.Data.Type, e.Data.From, request });
						break;
					}
					case MessageType::DisconnectRequest:
					{
						ServerDisconnectRequest request;
						Deserialize(e.Data.Data, request);
						OnDisconnectRequest({ e.Data.Type, e.Data.From, request });
						break;
					}
				}
			});
	}

	void ServerListener::OnConnectionRequest(const ServerMessage<ServerConnectionRequest>& request)
	{
		ClientConnection& connection = ServerState::Get().GetConnections().AddConnection(request.Data.Username, request.From);
		ServerConnectionResponse response;
		response.ConnectionId = connection.GetConnectionId();
		response.Success = true;
		m_ServerSocket.SendPacket(request.From, MessageType::ConnectResponse, response);
		BLT_TRACE("Client Connection Accepted. Id: {}", response.ConnectionId);
	}

	void ServerListener::OnDisconnectRequest(const ServerMessage<ServerDisconnectRequest>& request)
	{
		bool success = ServerState::Get().GetConnections().RemoveConnection(request.Data.ConnectionId);
		if (success)
		{
			ForceDisconnectMessage message;
			m_ServerSocket.SendPacket(request.From, MessageType::ForceDisconnect, message);
			BLT_TRACE("Sending Disconnect Message. Id: {}", request.Data.ConnectionId);
		}
	}

}