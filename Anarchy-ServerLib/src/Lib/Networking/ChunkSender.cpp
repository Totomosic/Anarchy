#include "ChunkSender.h"
#include "Lib/MessageType.h"

namespace Anarchy
{

	ChunkSender::ChunkSender(UDPsocket* socket)
		: m_InProgressChunks(), m_NextChunkId(0), m_Socket(socket), m_StartTime()
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
	}

	bool ChunkSender::CanSendPacket() const
	{
		return true;
	}

	void ChunkSender::SendPacket(const SocketAddress& to, const void* data, uint32_t size)
	{
		BLT_ASSERT(size <= MaxChunkSize, "Packet too large");
		if (CanSendPacket())
		{
			chunkid_t id = m_NextChunkId++;
			size_t index = m_InProgressChunks.size();
			m_InProgressChunks.resize(index + 1);
			ChunkData& chunk = m_InProgressChunks[index];
			chunk.ChunkSize = size;
			chunk.CurrentSliceIndex = 0;
			chunk.NumSlices = CalculateNumSlices(size);
			chunk.ReceiverAddress = to;
			chunk.ChunkId = id;
			chunk.NumAckedSlices = 0;
			memset(chunk.Acked, 0, sizeof(chunk.Acked));
			memset(chunk.TimeLastSent, 0, sizeof(chunk.TimeLastSent));
			memcpy(chunk.ChunkData, data, size);
		}
	}

	void ChunkSender::Update(TimeDelta dt)
	{
		size_t timeElapsedMilliseconds = 100;
		size_t maxBytes = (size_t)(1 * 1024 * 1024 * dt.Seconds());
		size_t totalSentBytes = 0;
		bool sent = true;
		while (totalSentBytes < maxBytes && sent)
		{
			sent = false;
			for (ChunkData& chunk : m_InProgressChunks)
			{
				size_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
				if (!chunk.Acked[chunk.CurrentSliceIndex] && chunk.TimeLastSent[chunk.CurrentSliceIndex] + timeElapsedMilliseconds < timestamp)
				{
					chunk.TimeLastSent[chunk.CurrentSliceIndex] = timestamp;
					int sliceSize = (chunk.CurrentSliceIndex == chunk.NumSlices - 1) ? CalculateFinalChunkSize(chunk.ChunkSize) : MaxSliceSize;
					ChunkSlicePacket packet;
					packet.ChunkId = chunk.ChunkId;
					packet.NumSlices = chunk.NumSlices;
					packet.SliceIndex = chunk.CurrentSliceIndex;
					packet.SliceBytes = sliceSize;
					memcpy(packet.Data, chunk.ChunkData + packet.SliceIndex * MaxSliceSize, sliceSize);

					OutputMemoryStream stream(sizeof(ChunkSlicePacket));
					Serialize(stream, MessageType::ChunkSlice);
					Serialize(stream, packet);
					m_Socket->SendTo(chunk.ReceiverAddress, stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());

					totalSentBytes += sliceSize;
					sent = true;
				}
				chunk.CurrentSliceIndex = (chunk.CurrentSliceIndex + 1) % chunk.NumSlices;
				if (totalSentBytes >= maxBytes)
					break;
			}
		}
	}

	void ChunkSender::HandleAckPacket(InputMemoryStream& stream)
	{
		ChunkAckPacket packet;
		Deserialize(stream, packet);
		for (int i = 0; i < m_InProgressChunks.size(); i++)
		{
			ChunkData& chunk = m_InProgressChunks[i];
			if (chunk.ChunkId == packet.ChunkId && chunk.NumSlices == packet.NumSlices)
			{
				for (int sliceIndex = 0; sliceIndex < chunk.NumSlices; sliceIndex++)
				{
					if (!chunk.Acked[sliceIndex] && packet.Acked[sliceIndex])
					{
						chunk.Acked[sliceIndex] = true;
						chunk.NumAckedSlices += 1;
						if (chunk.NumAckedSlices == chunk.NumSlices)
						{
							m_InProgressChunks.erase(m_InProgressChunks.begin() + i);
						}
					}
				}
				break;
			}
		}
	}

	int ChunkSender::CalculateNumSlices(uint32_t size) const
	{
		return size / MaxSliceSize + ((size % MaxSliceSize == 0) ? 0 : 1);
	}

	uint16_t ChunkSender::CalculateFinalChunkSize(uint32_t size) const
	{
		return (uint16_t)size - (uint16_t)((size / MaxSliceSize) * MaxSliceSize);
	}

}
