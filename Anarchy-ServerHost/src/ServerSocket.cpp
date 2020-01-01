#include "serverpch.h"
#include "ServerSocket.h"

namespace Anarchy
{



	ServerSocket::ServerSocket(const SocketAddress& address)
		: m_Address(address), m_Socket(), m_Bus(), m_OnMessage(m_Bus.GetEmitter<ClientMessageReceived>(ServerEvents::ClientMessageRecevied))
	{
		
	}

	const SocketAddress& ServerSocket::GetAddress() const
	{
		return m_Address;
	}

	const UDPsocket& ServerSocket::GetSocket() const
	{
		return m_Socket;
	}

	EventEmitter<ClientMessageReceived>& ServerSocket::OnMessageReceived()
	{
		return m_OnMessage;
	}

	void ServerSocket::Run()
	{
		m_Socket.Bind(m_Address);
		Task listenerThread = TaskManager::Run([this]()
			{
				std::byte buffer[4096];
				while (true)
				{
					SocketAddress from;
					int received = m_Socket.RecvFrom(buffer, sizeof(buffer), &from);
					if (received > 0)
					{
						InputMemoryStream stream(received);
						memcpy(stream.GetBufferPtr(), buffer, received);
						ClientMessageReceived e;
						e.From = from;
						Deserialize(stream, e.Type);
						e.Data = std::move(stream);
						OnMessageReceived().Emit(std::move(e));
					}
				}
			});		
	}

}