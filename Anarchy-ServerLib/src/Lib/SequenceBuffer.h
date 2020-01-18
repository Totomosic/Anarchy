#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	struct PacketData
	{
	public:
		using timestamp_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

	public:
		bool Acked = false;
		timestamp_t Timestamp;
	};

	class SequenceBuffer
	{
	private:
		static constexpr uint32_t BufferSize = 1024;

		seqid_t m_Sequences[BufferSize];
		PacketData m_Data[BufferSize];

	public:
		SequenceBuffer();

		const PacketData* GetPacketData(seqid_t sequenceId) const;
		PacketData* GetPacketData(seqid_t sequenceId);
		PacketData& InsertPacketData(seqid_t sequenceId);

	private:
		uint32_t GetIndex(seqid_t sequenceId) const;
	};

}