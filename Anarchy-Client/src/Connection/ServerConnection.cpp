#include "clientpch.h"
#include "ServerConnection.h"
#include "Events.h"

namespace Anarchy
{

	ServerConnection::ServerConnection(const SocketAddress& address)
		: m_Address(address), m_Socket(), m_Bus(), m_OnMessage(m_Bus.GetEmitter<ServerMessageReceived>(ServerEvents::ServerMessageReceived)), m_IsValid(true)
	{
		LaunchListenerThread();
	}

	ServerConnection::~ServerConnection()
	{
		m_IsValid = false;
	}

	const SocketAddress& ServerConnection::GetAddress() const
	{
		return m_Address;
	}

	const UDPsocket& ServerConnection::GetSocket() const
	{
		return m_Socket;
	}

	EventEmitter<ServerMessageReceived>& ServerConnection::OnMessageReceived()
	{
		return m_OnMessage;
	}

	Task<ServerConnectionResponse> ServerConnection::Connect(const ServerConnectionRequest& request)
	{
		Task<ServerConnectionResponse> response = TaskManager::Run([this, request]()
			{
				std::promise<ServerConnectionResponse> result;
				ScopedEventListener listener = OnMessageReceived().AddScopedEventListener([&result](Event<ServerMessageReceived>& e)
					{
						if (e.Data.Type == MessageType::ConnectResponse)
						{
							ServerConnectionResponse response;
							Deserialize(e.Data.Data, response);
							result.set_value(response);
						}
					});
				SendPacket(MessageType::ConnectRequest, request);
				auto future = result.get_future();
				std::future_status status = future.wait_for(std::chrono::seconds(5));
				if (status == std::future_status::ready)
				{
					return future.get();
				}
				ServerConnectionResponse response;
				response.Success = false;
				return response;
			});
		return response;
	}

	void ServerConnection::RequestDisconnect(uint64_t connectionId)
	{
		ServerDisconnectRequest request;
		request.ConnectionId = connectionId;
		SendPacket(MessageType::DisconnectRequest, request);
	}

	void ServerConnection::LaunchListenerThread()
	{
		int port = 10001;
		int result = m_Socket.Bind(SocketAddress("localhost", port));
		while (result != 0)
		{
			result = m_Socket.Bind(SocketAddress("localhost", ++port));
		}
		Task task = TaskManager::Run([this]()
			{
				std::byte buffer[4096];
				while (m_IsValid)
				{
					SocketAddress from;
					int received = m_Socket.RecvFrom(buffer, sizeof(buffer), &from);
					if (received > 0)
					{
						InputMemoryStream stream(received);
						memcpy(stream.GetBufferPtr(), buffer, received);
						ServerMessageReceived e;
						Deserialize(stream, e.Type);
						e.Data = std::move(stream);
						OnMessageReceived().Emit(std::move(e));
					}
					else
					{
						break;
					}
				}
			});
	}

}