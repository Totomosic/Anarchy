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
			size_t TimeLastSentMilliseconds[MaxSlicesPerChunk];
			size_t MillisecondsUntilResend;
			SocketAddress ReceiverAddress;
			int MaxRetries;
		};

	private:
		std::vector<ChunkData> m_InProgressChunks;
		chunkid_t m_NextChunkId;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;

		size_t m_MaxBytesPerSecond;
		int64_t m_BytesSent;

		UDPsocket* m_Socket;

	public:
		ChunkSender(UDPsocket* socket);

		size_t GetMaxBytesPerSecond() const;
		bool CanSendPacket() const;
		void SetMaxBytesPerSecond(size_t bytesPerSecond);

		// Send the data block as a stream of slices
		// Use millisecondsForResend to account for varying RTT between clients (a good default is RTT * 1.25)
		void SendPacket(const SocketAddress& to, const void* data, uint32_t size, size_t millisecondsForResend = 100);

		void Update(TimeDelta dt);
		void HandleAckPacket(InputMemoryStream& stream);

	private:
		int CalculateNumSlices(uint32_t size) const;
		uint16_t CalculateFinalChunkSize(uint32_t size) const;
	};

}