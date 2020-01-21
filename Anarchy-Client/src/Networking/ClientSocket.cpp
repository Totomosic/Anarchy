#include "clientpch.h"
#include "ClientSocket.h"
#include "Events.h"

#include "Engine/Networking/SocketUtil.h"

namespace Anarchy
{

	ClientSocket::ClientSocket(const SocketAddress& address)
		: m_Address(address), m_Socket(), m_Bus(), m_OnMessage(m_Bus.GetEmitter<ServerMessageReceived>(ServerEvents::ServerMessageReceived))
	{
		m_Bus.SetImmediateMode(true);
		m_Socket.Connect(m_Address);
		LaunchListenerThread();
	}

	const SocketAddress& ClientSocket::GetAddress() const
	{
		return m_Address;
	}

	const UDPsocket& ClientSocket::GetSocket() const
	{
		return m_Socket;
	}

	EventEmitter<ServerMessageReceived>& ClientSocket::OnMessageReceived()
	{
		return m_OnMessage;
	}

	void ClientSocket::LaunchListenerThread()
	{
		Task task = TaskManager::Get().Run([this]()
			{
				std::byte buffer[MaxPacketSize];
				while (true)
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