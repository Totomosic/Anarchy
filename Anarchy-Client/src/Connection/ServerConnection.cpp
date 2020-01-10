#include "clientpch.h"
#include "ServerConnection.h"
#include "Events.h"

#include "Engine/Networking/SocketUtil.h"

namespace Anarchy
{

	ServerConnection::ServerConnection(const SocketAddress& address)
		: m_Address(address), m_Socket(), m_Bus(), m_OnMessage(m_Bus.GetEmitter<ServerMessageReceived>(ServerEvents::ServerMessageReceived)), m_IsValid(true)
	{
		m_Bus.SetImmediateMode(true);
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

	void ServerConnection::LaunchListenerThread()
	{
		std::vector<uint32_t> addresses = SocketUtil::GetIP4Addresses();
		if (addresses.size() > 0)
		{
			int port = 10001;
			int result = m_Socket.Bind(SocketAddress(addresses[0], port));
			while (result != 0)
			{
				result = m_Socket.Bind(SocketAddress(addresses[0], ++port));
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

}