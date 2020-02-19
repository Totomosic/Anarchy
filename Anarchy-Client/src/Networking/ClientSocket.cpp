#include "clientpch.h"
#include "ClientSocket.h"
#include "Events.h"

#include "Engine/Networking/SocketUtil.h"

namespace Anarchy
{

	ClientSocket::ClientSocket(const SocketAddress& address)
		: m_Address(address), m_Socket(), m_ChunkSender(&m_Socket), m_ChunkReceiver(&m_Socket), m_Bus(), m_OnMessage(m_Bus.GetEmitter<ServerMessageReceived>(ServerEvents::ServerMessageReceived)), m_ShutdownListener(false)
	{
		m_Bus.SetImmediateMode(true);
		m_Socket.Connect(m_Address);
		m_Socket.SetBlocking(false);
		LaunchListenerThread();
	}

	ClientSocket::~ClientSocket()
	{
		m_ShutdownListener = true;
		while (m_ShutdownListener) {}
		m_Bus.Flush();
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

	void ClientSocket::Update(TimeDelta dt)
	{
		m_ChunkSender.Update(dt);
	}

	void ClientSocket::LaunchListenerThread()
	{
		Task task = TaskManager::Get().Run([this]()
			{
				std::byte buffer[MaxPacketSize];
				while (!m_ShutdownListener)
				{
					UDPsocket* socket = SocketUtil::SelectRead(&m_Socket, 200);
					if (socket != nullptr)
					{
						SocketAddress from;
						int received = socket->RecvFrom(buffer, sizeof(buffer), &from);
						if (received > 0)
						{
							InputMemoryStream stream(received);
							memcpy(stream.GetBufferPtr(), buffer, received);
							ServerMessageReceived e;
							Deserialize(stream, e.Type);
							e.Data = std::move(stream);
							if (e.Type == MessageType::ChunkSlice)
							{
								std::optional<InputMemoryStream> result = m_ChunkReceiver.HandleSlicePacket(from, e.Data);
								if (result)
								{
									Deserialize(*result, e.Type);
									e.Data = std::move(*result);
									OnMessageReceived().Emit(std::move(e));
								}
							}
							else if (e.Type == MessageType::ChunkAck)
							{
								m_ChunkSender.HandleAckPacket(e.Data);
							}
							else
							{
								OnMessageReceived().Emit(std::move(e));
							}
						}
					}
				}
				BLT_INFO("Stopped Listener Thread");
				m_ShutdownListener = false;
			});
	}

	void ClientSocket::HardResetStream(InputMemoryStream& stream) const
	{
		InputMemoryStream result(stream.GetRemainingDataSize());
		memcpy(result.GetBufferPtr(), stream.GetBufferPtr(), stream.GetRemainingDataSize());
		stream = std::move(result);
	}

}