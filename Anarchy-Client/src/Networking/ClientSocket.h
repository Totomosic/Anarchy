#pragma once
#include "Lib/RequestMessages.h"
#include "Lib/Networking/ChunkSender.h"
#include "Lib/Networking/ChunkReceiver.h"
#include "Core/Events/EventEmitter.h"
#include "Core/Events/EventBus.h"

namespace Anarchy
{

	struct ServerMessageReceived
	{
	public:
		MessageType Type;
		InputMemoryStream Data;
	};

	class ClientSocket
	{
	private:
		SocketAddress m_Address;
		UDPsocket m_Socket;
		ChunkSender m_ChunkSender;
		ChunkReceiver m_ChunkReceiver;

		EventBus m_Bus;
		EventEmitter<ServerMessageReceived> m_OnMessage;

	public:
		ClientSocket(const SocketAddress& address);
		ClientSocket(const ClientSocket& other) = delete;
		ClientSocket& operator=(const ClientSocket& other) = delete;
		ClientSocket(ClientSocket&& other) = delete;
		ClientSocket& operator=(ClientSocket&& other) = delete;
		~ClientSocket() = default;

		const SocketAddress& GetAddress() const;
		const UDPsocket& GetSocket() const;
		EventEmitter<ServerMessageReceived>& OnMessageReceived();

		void Update(TimeDelta dt);

		template<typename ...Args>
		void SendPacket(MessageType type, Args&&... data)
		{
			OutputMemoryStream stream;
			Serialize(stream, type);
			(Serialize(stream, std::forward<Args>(data)), ...);
			if (stream.GetRemainingDataSize() <= MaxPacketSize)
			{
				m_Socket.SendTo(m_Address, (const void*)stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());
			}
			else
			{
				m_ChunkSender.SendPacket(m_Address, (const void*)stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());
			}
		}

	private:
		void LaunchListenerThread();
		void HardResetStream(InputMemoryStream& stream) const;

	};

}