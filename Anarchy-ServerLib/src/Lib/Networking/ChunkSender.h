#pragma once
#include "DataChunk.h"
#include "Core/Time/TimeDelta.h"

namespace Anarchy
{

	class ChunkSender
	{
	private:
		struct ChunkData
		{
		public:
			chunkid_t ChunkId = 0;
			uint32_t ChunkSize = 0;
			int NumSlices = 0;
			int CurrentSliceIndex = 0;
			int NumAckedSlices = 0;
			bool Acked[MaxSlicesPerChunk];
			uint8_t ChunkData[MaxChunkSize];
			double TimeLastSent[MaxSlicesPerChunk];
			SocketAddress ReceiverAddress;
		};

	private:
		std::vector<ChunkData> m_InProgressChunks;
		chunkid_t m_NextChunkId;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;

		UDPsocket* m_Socket;

	public:
		ChunkSender(UDPsocket* socket);

		bool CanSendPacket() const;
		void SendPacket(const SocketAddress& to, const void* data, uint32_t size);

		void Update(TimeDelta dt);
		void HandleAckPacket(InputMemoryStream& stream);

	private:
		int CalculateNumSlices(uint32_t size) const;
		uint16_t CalculateFinalChunkSize(uint32_t size) const;
	};

}