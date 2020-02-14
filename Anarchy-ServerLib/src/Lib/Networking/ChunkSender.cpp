#include "ChunkSender.h"

namespace Anarchy
{

	ChunkSender::ChunkSender(UDPsocket* socket)
		: m_Sending(false), m_ChunkId(0), m_ChunkSize(0), m_NumSlices(0), m_CurrentSliceId(0), m_Acked{}, m_ChunkData(), m_TimeLastSent(), m_Socket(socket), m_ReceiverAddress()
	{
	}

	bool ChunkSender::CanSendPacket() const
	{
		return !m_Sending;
	}

	void ChunkSender::Update(TimeDelta dt)
	{
		if (m_Sending)
		{

		}
	}

	void ChunkSender::SendPacket(const SocketAddress& to, const void* data, uint32_t size)
	{
		if (!m_Sending)
		{
			BLT_ASSERT(size <= MaxChunkSize, "Packet too large");
			m_ReceiverAddress = to;
		}
	}

	int ChunkSender::CalculateNumSlices(uint32_t size) const
	{
		return size / MaxSliceSize + ((size % MaxSliceSize == 0) ? 0 : 1);
	}

}
