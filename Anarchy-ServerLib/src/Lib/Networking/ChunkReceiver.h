#pragma once
#include "DataChunk.h"

namespace Anarchy
{

	class ChunkReceiver
	{
	private:
		struct ChunkData
		{
		public:
			SocketAddress SenderAddress;
			chunkid_t ChunkId = 0;
			uint32_t ChunkSize = 0;
			bool ForceAckPreviousChunk = false;
			int PreviousChunkNumSlices = 0;
			int NumSlices = 0;
			int NumReceivedSlices = 0;
			bool Received[MaxSlicesPerChunk];
			uint8_t Data[MaxChunkSize];
			size_t LastReceivedTime;
		};

	private:
		std::vector<ChunkData> m_InProgressChunks;
		UDPsocket* m_Socket;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;

	public:
		ChunkReceiver(UDPsocket* socket);

		std::optional<InputMemoryStream> HandleSlicePacket(const SocketAddress& fromAddress, InputMemoryStream& stream);
		void Update();

	private:
		std::optional<InputMemoryStream> ProcessPacket(ChunkData& chunk, const ChunkSlicePacket& packet, int index);
		void SendAckPacket(ChunkData& data);
		size_t GetTimestamp() const;
	};

}