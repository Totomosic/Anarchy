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
		MessageType Type = MessageType::None;
		InputMemoryStream Data;
	};

	class ServerSocket
	{
	private:
		struct Packet
		{
		public:
			SocketAddress Address;
			std::shared_ptr<void> Data;
			size_t Size;
		};

	private:
		SocketAddress m_Address;
		UDPsocket m_Socket;
		ChunkSender m_ChunkSender;
		ChunkReceiver m_ChunkReceiver;

		EventBus m_Bus;
		EventEmitter<ClientMessageReceived> m_OnMessage;

		std::deque<Packet> m_Packets;
		size_t m_MaxBytesPerSecond;
		size_t m_MaxBytes;
		size_t m_SentBytes;

	public:
		ServerSocket(const SocketAddress& address);
		ServerSocket(const ServerSocket& other) = delete;
		ServerSocket& operator=(const ServerSocket& other) = delete;
		ServerSocket(ServerSocket&& other) = delete;
		ServerSocket& operator=(ServerSocket&& other) = delete;
		~ServerSocket() = default;

		const SocketAddress& GetAddress() const;
		const UDPsocket& GetSocket() const;
		void SetMaxBytesPerSecond(size_t bytesPerSecond);

		EventEmitter<ClientMessageReceived>& OnMessageReceived();

		void Run();
		void Update(TimeDelta dt);

		template<typename ...Args>
		void SendPacket(const std::vector<SocketAddress>& addresses, MessageType type, Args&&... data)
		{
			OutputMemoryStream stream;
			Serialize(stream, type);
			(Serialize(stream, std::forward<Args>(data)), ...);
			if (stream.GetRemainingDataSize() <= MaxPacketSize || type == MessageType::ChunkAck || type == MessageType::ChunkSlice)
			{
				std::shared_ptr<void> data = std::shared_ptr<void>(new uint8_t[stream.GetRemainingDataSize()]);
				memcpy(data.get(), stream.GetBufferPtr(), stream.GetRemainingDataSize());
				for (const SocketAddress& to : addresses)
				{
					Packet packet;
					packet.Address = to;
					packet.Data = data;
					packet.Size = stream.GetRemainingDataSize();
					HandlePacket(packet);
				}
			}
			else
			{
				for (const SocketAddress& to : addresses)
				{
					m_ChunkSender.SendPacket(to, (const void*)stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());
				}
			}
		}

		template<typename ...Args>
		void SendPacket(const SocketAddress& to, MessageType type, Args&&... data)
		{
			OutputMemoryStream stream;
			Serialize(stream, type);
			(Serialize(stream, std::forward<Args>(data)), ...);
			if (stream.GetRemainingDataSize() <= MaxPacketSize || type == MessageType::ChunkAck || type == MessageType::ChunkSlice)
			{
				Packet packet;
				packet.Address = to;
				packet.Data = std::shared_ptr<void>(new uint8_t[stream.GetRemainingDataSize()]);
				packet.Size = stream.GetRemainingDataSize();
				memcpy(packet.Data.get(), stream.GetBufferPtr(), packet.Size);
				HandlePacket(packet);
			}
			else
			{
				m_ChunkSender.SendPacket(to, (const void*)stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());
			}
		}

	private:
		void HandlePacket(const Packet& packet);
		void SendPacket(const Packet& packet);

	};

}