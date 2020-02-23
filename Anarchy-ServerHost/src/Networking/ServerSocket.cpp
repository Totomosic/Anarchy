#include "serverpch.h"
#include "ServerSocket.h"
#include "Core/Tasks/TaskManager.h"

namespace Anarchy
{

	ServerSocket::ServerSocket(const SocketAddress& address)
		: m_Address(address), m_Socket(), m_ChunkSender(&m_Socket), m_ChunkReceiver(&m_Socket), m_Bus(), m_OnMessage(m_Bus.GetEmitter<ClientMessageReceived>(ServerEvents::ClientMessageRecevied)), 
		m_Packets(), m_MaxBytesPerSecond(1024 * 1024 * 1), m_MaxBytes(0), m_SentBytes(0)
	{
		m_Bus.SetImmediateMode(true);
	}

	const SocketAddress& ServerSocket::GetAddress() const
	{
		return m_Address;
	}

	const UDPsocket& ServerSocket::GetSocket() const
	{
		return m_Socket;
	}

	void ServerSocket::SetMaxBytesPerSecond(size_t bytesPerSecond)
	{
		m_MaxBytesPerSecond = bytesPerSecond;
		m_ChunkSender.SetMaxBytesPerSecond(bytesPerSecond);
	}

	EventEmitter<ClientMessageReceived>& ServerSocket::OnMessageReceived()
	{
		return m_OnMessage;
	}

	void ServerSocket::Run()
	{
		m_Socket.Bind(m_Address);
		Task listenerThread = TaskManager::Get().Run([this]()
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
						ClientMessageReceived e;
						e.From = from;
						Deserialize(stream, e.Type);
						e.Data = std::move(stream);
						if (e.Type == MessageType::ChunkSlice)
						{
							std::optional<InputMemoryStream> result = m_ChunkReceiver.HandleSlicePacket(e.From, e.Data);
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
			});		
	}

	void ServerSocket::Update(TimeDelta dt)
	{
		m_ChunkSender.Update(dt);
		m_MaxBytes = (size_t)(m_MaxBytesPerSecond * dt.Seconds()) + 1;
		if (m_MaxBytes >= m_SentBytes)
		{
			m_SentBytes = 0;
		}
		else
		{
			m_SentBytes -= m_MaxBytes;
		}
		while (!m_Packets.empty() && m_SentBytes < m_MaxBytes)
		{
			SendPacket(m_Packets.front());
			m_Packets.pop_front();
		}
	}

	void ServerSocket::HandlePacket(const Packet& packet)
	{
		if (m_SentBytes < m_MaxBytes)
		{
			SendPacket(packet);
		}
		else
		{
			m_Packets.push_back(packet);
		}
	}

	void ServerSocket::SendPacket(const Packet& packet)
	{
		m_Socket.SendTo(packet.Address, (const void*)packet.Data.get(), (uint32_t)packet.Size);
		m_SentBytes += packet.Size;
	}

}