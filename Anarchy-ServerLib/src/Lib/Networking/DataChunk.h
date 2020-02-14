#pragma once
#include "Utils/Serialization.h"
#include "Utils/Deserialization.h"

namespace Anarchy
{

	constexpr uint16_t MaxSlicesPerChunk = 256;
	constexpr uint16_t MaxSliceSize = 1024;
	constexpr uint32_t MaxChunkSize = (uint32_t)MaxSlicesPerChunk * (uint32_t)MaxSliceSize;

	using chunkid_t = uint16_t;

	struct ChunkSlicePacket
	{
	public:
		chunkid_t ChunkId;
		int SliceId;
		int NumSlices;
		int SliceBytes;
		uint8_t Data[MaxSliceSize];
	};

	inline void Serialize(OutputMemoryStream& stream, const ChunkSlicePacket& packet)
	{
		Serialize(stream, packet.ChunkId);
		Serialize(stream, packet.SliceId);
		Serialize(stream, packet.NumSlices);
		if (packet.SliceId == packet.NumSlices - 1)
		{
			Serialize(stream, packet.SliceBytes);
			stream.Write(packet.Data, packet.SliceBytes);
		}
		else
		{
			stream.Write(packet.Data, MaxSliceSize);
		}
	}

	inline void Deserialize(InputMemoryStream& stream, ChunkSlicePacket& packet)
	{
		Deserialize(stream, packet.ChunkId);
		Deserialize(stream, packet.SliceId);
		Deserialize(stream, packet.NumSlices);
		if (packet.SliceId == packet.NumSlices - 1)
		{
			Deserialize(stream, packet.SliceBytes);
			stream.Read(packet.Data, packet.SliceBytes);
		}
		else
		{
			stream.Read(packet.Data, MaxSliceSize);
		}
	}

	struct ChunkAckPacket
	{
	public:
		chunkid_t ChunkId;
		int NumSlices;
		bool Acked[MaxSlicesPerChunk];
	};

	inline void Serialize(OutputMemoryStream& stream, const ChunkAckPacket& packet)
	{
		Serialize(stream, packet.ChunkId);
		Serialize(stream, packet.NumSlices);
		constexpr int byteSize = 8;
		constexpr int numBytes = MaxSlicesPerChunk / byteSize + ((MaxSlicesPerChunk % byteSize == 0) ? 0 : 1);
		for (int i = 0; i < numBytes; i++)
		{
			uint8_t data = 0;
			for (int j = 0; j < byteSize; j++)
			{
				int index = j + i * byteSize;
				if (packet.Acked[index])
				{
					data |= 1U << j;
				}
			}
			Serialize(stream, data);
		}
	}

	inline void Deserialize(InputMemoryStream& stream, ChunkAckPacket& packet)
	{
		Deserialize(stream, packet.ChunkId);
		Deserialize(stream, packet.NumSlices);
		constexpr int byteSize = 8;
		constexpr int numBytes = MaxSlicesPerChunk / byteSize + ((MaxSlicesPerChunk % byteSize == 0) ? 0 : 1);
		for (int i = 0; i < numBytes; i++)
		{
			uint8_t data = 0;
			Deserialize(stream, data);
			for (int j = 0; j < byteSize; j++)
			{
				int index = j + i * byteSize;
				if ((data >> j) & 1U)
				{
					packet.Acked[index] = true;
				}
				else
				{
					packet.Acked[index] = false;
				}
			}			
		}
	}

}