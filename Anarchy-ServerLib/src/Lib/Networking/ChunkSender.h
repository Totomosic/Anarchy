#pragma once
#include "DataChunk.h"
#include "Core/Time/TimeDelta.h"

namespace Anarchy
{

	class ChunkSender
	{
	private:
		bool m_Sending;
		chunkid_t m_ChunkId;
		uint32_t m_ChunkSize;
		int m_NumSlices;
		int m_CurrentSliceId;
		bool m_Acked[MaxSlicesPerChunk];
		uint8_t m_ChunkData[MaxChunkSize];
		double m_TimeLastSent[MaxSlicesPerChunk];

		UDPsocket* m_Socket;
		SocketAddress m_ReceiverAddress;

	public:
		ChunkSender(UDPsocket* socket);

		bool CanSendPacket() const;

		void Update(TimeDelta dt);
		void SendPacket(const SocketAddress& to, const void* data, uint32_t size);

	private:
		int CalculateNumSlices(uint32_t size) const;
	};

}