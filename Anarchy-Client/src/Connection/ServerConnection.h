#pragma once
#include "Lib/Authentication.h"

namespace Anarchy
{

	struct ServerMessageReceived
	{
	public:
		MessageType Type;
		InputMemoryStream Data;
	};

	class ServerConnection
	{
	private:
		SocketAddress m_Address;
		UDPsocket m_Socket;
		EventBus m_Bus;
		EventEmitter<ServerMessageReceived> m_OnMessage;

	public:
		ServerConnection(const SocketAddress& address);
		ServerConnection(const ServerConnection& other) = delete;
		ServerConnection& operator=(const ServerConnection& other) = delete;
		ServerConnection(ServerConnection&& other) = delete;
		ServerConnection& operator=(ServerConnection&& other) = delete;

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