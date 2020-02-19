#include "ChunkSender.h"
#include "Lib/MessageType.h"

namespace Anarchy
{

	ChunkSender::ChunkSender(UDPsocket* socket)
		: m_InProgressChunks(), m_NextChunkId(0), m_MaxBytesPerSecond(1024 * 1024 / 8), m_BytesSent(0), m_Socket(socket), m_StartTime()
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
	}

	size_t ChunkSender::GetMaxBytesPerSecond() const
	{
		return m_MaxBytesPerSecond;
	}

	bool ChunkSender::CanSendPacket() const
	{
		return true;
	}

	void ChunkSender::SetMaxBytesPerSecond(size_t bytesPerSecond)
	{
		m_MaxBytesPerSecond = bytesPerSecond;
		m_BytesSent = 0;
		m_StartTime = std::chrono::high_resolution_clock::now();
	}

	void ChunkSender::SendPacket(const SocketAddress& to, const void* data, uint32_t size, size_t millisecondsForResend)
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
			chunk.MillisecondsUntilResend = millisecondsForResend;
			chunk.MaxRetries = 10;
			memset(chunk.Acked, 0, sizeof(chunk.Acked));
			memset(chunk.TimeLastSentMilliseconds, 0, sizeof(chunk.TimeLastSentMilliseconds));
			memcpy(chunk.ChunkData, data, size);
		}
	}

	void ChunkSender::Update(TimeDelta dt)
	{
		size_t maxBytes = (size_t)(GetMaxBytesPerSecond() * dt.Seconds()) + 1;
		m_BytesSent = std::max(m_BytesSent - int64_t(maxBytes), int64_t(0));
		bool sent = true;
		while (m_BytesSent < maxBytes && sent)
		{
			sent = false;
			for (int i = 0; i < m_InProgressChunks.size(); i++)
			{
				ChunkData& chunk = m_InProgressChunks[i];
				size_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
				if (!chunk.Acked[chunk.CurrentSliceIndex] && chunk.TimeLastSentMilliseconds[chunk.CurrentSliceIndex] + chunk.MillisecondsUntilResend < timestamp)
				{
					chunk.TimeLastSentMilliseconds[chunk.CurrentSliceIndex] = timestamp;
					int sliceSize = (chunk.CurrentSliceIndex == chunk.NumSlices - 1) ? CalculateFinalChunkSize(chunk.ChunkSize) : MaxSliceSize;
					ChunkSlicePacket packet;
					packet.ChunkId = chunk.ChunkId;
					packet.NumSlices = chunk.NumSlices;
					packet.SliceIndex = chunk.CurrentSliceIndex;
					packet.SliceBytes = sliceSize;
					memcpy(packet.Data, chunk.ChunkData + (int64_t)packet.SliceIndex * MaxSliceSize, sliceSize);

					OutputMemoryStream stream(sizeof(ChunkSlicePacket));
					Serialize(stream, MessageType::ChunkSlice);
					Serialize(stream, packet);
					m_Socket->SendTo(chunk.ReceiverAddress, stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());

					m_BytesSent += sliceSize;
					sent = true;

					if (chunk.CurrentSliceIndex >= chunk.NumSlices - 1)
					{
						chunk.MaxRetries -= 1;
						if (chunk.MaxRetries <= 0)
						{
							BLT_INFO("Max retries exceeded, Chunk {}", chunk.ChunkId);
							m_InProgressChunks.erase(m_InProgressChunks.begin() + i);
							break;
						}
					}
				}
				chunk.CurrentSliceIndex = (chunk.CurrentSliceIndex + 1) % chunk.NumSlices;
				if (m_BytesSent >= maxBytes)
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
