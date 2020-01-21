#pragma once
#include "Lib/Authentication.h"
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

		template<typename T>
		void SendPacket(MessageType type, const T& data)
		{
			OutputMemoryStream stream;
			Serialize(stream, type);
			Serialize(stream, data);
			BLT_ASSERT(stream.GetRemainingDataSize() <= MaxPacketSize, "Packet too large");
			m_Socket.SendTo(m_Address, (const void*)stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());
		}

	private:
		void LaunchListenerThread();

	};

}