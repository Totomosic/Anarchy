#include "ChunkReceiver.h"
#include "Lib/MessageType.h"

namespace Anarchy
{

	ChunkReceiver::ChunkReceiver(UDPsocket* socket)
		: m_InProgressChunks(), m_Socket(socket)
	{
	}

	std::optional<InputMemoryStream> ChunkReceiver::HandleSlicePacket(const SocketAddress& fromAddress, InputMemoryStream& stream)
	{
		ChunkSlicePacket packet;
		Deserialize(stream, packet);
		for (int i = 0; i < m_InProgressChunks.size(); i++)
		{
			ChunkData& chunk = m_InProgressChunks[i];
			if (chunk.ChunkId == packet.ChunkId && chunk.SenderAddress == fromAddress && chunk.NumSlices == packet.NumSlices)
			{
				return ProcessPacket(chunk, packet, i);
			}
		}
		// New Chunk
		size_t index = m_InProgressChunks.size();
		m_InProgressChunks.resize(index + 1);
		ChunkData& chunk = m_InProgressChunks[index];
		chunk.ChunkId = packet.ChunkId;
		chunk.ChunkSize = 0;
		chunk.ForceAckPreviousChunk = false;
		chunk.NumReceivedSlices = 0;
		chunk.NumSlices = packet.NumSlices;
		chunk.PreviousChunkNumSlices = 0;
		chunk.SenderAddress = fromAddress;
		memset(chunk.Received, 0, sizeof(chunk.Received));
		return ProcessPacket(chunk, packet, index);
	}

	std::optional<InputMemoryStream> ChunkReceiver::ProcessPacket(ChunkData& chunk, const ChunkSlicePacket& packet, int index)
	{
		if (!chunk.Received[packet.SliceIndex])
		{
			chunk.NumReceivedSlices += 1;
			chunk.Received[packet.SliceIndex] = true;
			SendAckPacket(chunk);
			BLT_ASSERT(packet.SliceBytes > 0 && packet.SliceBytes <= MaxSliceSize, "Invalid slice size");
			memcpy(chunk.Data + (int64_t)packet.SliceIndex * MaxSliceSize, packet.Data, packet.SliceBytes);
			if (packet.SliceIndex == chunk.NumSlices - 1)
			{
				chunk.ChunkSize = chunk.NumSlices * MaxSliceSize + packet.SliceBytes;
			}
			if (chunk.NumReceivedSlices == chunk.NumSlices)
			{
				InputMemoryStream result(chunk.ChunkSize);
				memcpy(result.GetBufferPtr(), chunk.Data, chunk.ChunkSize + (int64_t)index);
				return std::move(result);
			}
		}
		return {};
	}

	void ChunkReceiver::SendAckPacket(ChunkData& data)
	{
		ChunkAckPacket packet;
		packet.ChunkId = data.ChunkId;
		packet.NumSlices = data.NumSlices;
		for (int i = 0; i < data.NumSlices; i++)
		{
			packet.Acked[i] = data.Received[i];
		}
		OutputMemoryStream stream(sizeof(ChunkAckPacket));
		Serialize(stream, MessageType::ChunkAck);
		Serialize(stream, packet);
		m_Socket->SendTo(data.SenderAddress, stream.GetBufferPtr(), (uint32_t)stream.GetRemainingDataSize());
	}

}
