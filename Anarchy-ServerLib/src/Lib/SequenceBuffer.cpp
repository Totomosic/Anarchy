#include "SequenceBuffer.h"

namespace Anarchy
{

	SequenceBuffer::SequenceBuffer()
		: m_Sequences{}, m_Data{}
	{
		for (seqid_t i = 0; i < BufferSize; i++)
		{
			m_Sequences[i] = (i % BufferSize) + 1;
		}
	}

	const PacketData* SequenceBuffer::GetPacketData(seqid_t sequenceId) const
	{
		uint32_t index = GetIndex(sequenceId);
		if (m_Sequences[index] == sequenceId)
		{
			return &m_Data[index];
		}
		return nullptr;
	}

	PacketData* SequenceBuffer::GetPacketData(seqid_t sequenceId)
	{
		uint32_t index = GetIndex(sequenceId);
		if (m_Sequences[index] == sequenceId)
		{
			return &m_Data[index];
		}
		return nullptr;
	}

	PacketData& SequenceBuffer::InsertPacketData(seqid_t sequenceId)
	{
		uint32_t index = GetIndex(sequenceId);
		m_Sequences[index] = sequenceId;
		m_Data[index] = {};
		return m_Data[index];
	}

	uint32_t SequenceBuffer::GetIndex(seqid_t sequenceId) const
	{
		return sequenceId % BufferSize;
	}

}