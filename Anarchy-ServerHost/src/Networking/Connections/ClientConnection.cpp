#include "serverpch.h"
#include "ClientConnection.h"

namespace Anarchy
{

	ClientConnection::ClientConnection() : ClientConnection("", InvalidConnectionId, {})
	{
	}

	ClientConnection::ClientConnection(const std::string& username, connid_t connectionId, const SocketAddress& address)
		: m_Username(username), m_ConnectionId(connectionId), m_Address(address), m_SequenceId(0), m_RemoteSequenceId(0), m_MillisecondsSinceReceivedPacket(0), m_MillisecondsSinceSentPacket(0), m_AverageRTT(0),
		m_ReceivedMessages(), m_SentMessages()
	{
	}

	const std::string& ClientConnection::GetUsername() const
	{
		return m_Username;
	}

	connid_t ClientConnection::GetConnectionId() const
	{
		return m_ConnectionId;
	}

	const SocketAddress& ClientConnection::GetAddress() const
	{
		return m_Address;
	}

	seqid_t ClientConnection::GetSequenceId() const
	{
		return m_SequenceId;
	}

	void ClientConnection::SetSequenceId(seqid_t sequenceId)
	{
		if (IsSeqIdGreater(sequenceId, m_SequenceId))
		{
			m_SequenceId = sequenceId;
		}
	}

	void ClientConnection::IncrementSequenceId(seqid_t amount)
	{
		SetSequenceId(GetSequenceId() + amount);
	}

	seqid_t ClientConnection::GetRemoteSequenceId() const
	{
		return m_RemoteSequenceId;
	}

	void ClientConnection::SetRemoteSequenceId(seqid_t sequenceId)
	{
		if (IsSeqIdGreater(sequenceId, m_RemoteSequenceId))
		{
			m_RemoteSequenceId = sequenceId;
		}
	}

	uint32_t ClientConnection::GetTimeSinceLastPacket() const
	{
		return (uint32_t)m_MillisecondsSinceReceivedPacket;
	}

	void ClientConnection::ResetTimeSinceLastPacket()
	{
		m_MillisecondsSinceReceivedPacket = 0.0;
	}

	void ClientConnection::UpdateTimeSinceLastPacket(TimeDelta delta)
	{
		m_MillisecondsSinceReceivedPacket += delta.Milliseconds();
	}

	uint32_t ClientConnection::GetTimeSinceLastSentPacket() const
	{
		return m_MillisecondsSinceSentPacket;
	}

	void ClientConnection::ResetTimeSinceLastSentPacket()
	{
		m_MillisecondsSinceSentPacket = 0.0;
	}

	void ClientConnection::UpdateTimeSinceLastSentPacket(TimeDelta delta)
	{
		m_MillisecondsSinceSentPacket += delta.Milliseconds();
	}

	uint32_t ClientConnection::GetAverageRTT() const
	{
		return (uint32_t)m_AverageRTT;
	}

	void ClientConnection::UpdateAverageRTT(uint32_t rtt)
	{
		double delta = ((rtt - m_AverageRTT) * 0.1f);
		m_AverageRTT += delta;
		if (m_AverageRTT < 0)
		{
			m_AverageRTT = 0;
		}
	}

	SequenceBuffer& ClientConnection::GetReceivedBuffer()
	{
		return m_ReceivedMessages;
	}

	SequenceBuffer& ClientConnection::GetSentBuffer()
	{
		return m_SentMessages;
	}

}