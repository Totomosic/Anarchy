#pragma once
#include "serverpch.h"
#include "ServerLib.h"
#include "Events.h"
#include "Core/Events/EventEmitter.h"
#include "Core/Events/EventBus.h"
#include "Lib/Networking/ChunkReceiver.h"
#include "Lib/Networking/ChunkSender.h"

namespace Anarchy
{

	struct ClientMessageReceived
	{
	public:
		SocketAddress From;
		MessageType Type;
		InputMemoryStream Data;
	};

	class ServerSocket
	{
	private:
		SocketAddress m_Address;
		UDPsocket m_Socket;
		ChunkSender m_ChunkSender;
		ChunkReceiver m_ChunkReceiver;

		EventBus m_Bus;
		EventEmitter<ClientMessageReceived> m_OnMessage;

	public:
		ServerSocket(const SocketAddress& address);
		ServerSocket(const ServerSocket& other) = delete;
		ServerSocket& operator=(const ServerSocket& other) = delete;
		ServerSocket(ServerSocket&& other) = delete;
		ServerSocket& operator=(ServerSocket&& other) = delete;
		~ServerSocket() = default;

		const SocketAddress& GetAddress() const;
		const UDPsocket& GetSocket() const;

		EventEmitter<ClientMessageReceived>& OnMessageReceived();

		void Run();
		void Update(TimeDelta dt);

		template<typename T>
		void SendPacket(const std::vector<SocketAddress>& addresses, MessageType type, const T& data)
		{
			OutputMemoryStream stream;
			Serialize(stream, type);
			Serialize(stream, data);
			for (const SocketAddress& to : addresses)
			{
				m_Socket.SendTo(to, (const void*)stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());
			}
		}

		template<typename T>
		void SendPacket(const SocketAddress& to, MessageType type, const T& data)
		{
			OutputMemoryStream stream;
			Serialize(stream, type);
			Serialize(stream, data);
			m_Socket.SendTo(to, (const void*)stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());
		}
	};

}