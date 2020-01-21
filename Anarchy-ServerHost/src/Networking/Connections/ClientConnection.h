#pragma once
#include "serverpch.h"
#include "ServerLib.h"
#include "Lib/SequenceBuffer.h"
#include "Core/Time/TimeDelta.h"

namespace Anarchy
{

	class ClientConnection
	{
	private:
		std::string m_Username;
		connid_t m_ConnectionId;
		SocketAddress m_Address;
		seqid_t m_SequenceId;
		seqid_t m_RemoteSequenceId;

		double m_MillisecondsSinceReceivedPacket;
		double m_MillisecondsSinceSentPacket;
		double m_AverageRTT;

		SequenceBuffer m_ReceivedMessages;
		SequenceBuffer m_SentMessages;

	public:
		ClientConnection();
		ClientConnection(const std::string& username, connid_t connectionId, const SocketAddress& address);

		const std::string& GetUsername() const;
		connid_t GetConnectionId() const;
		const SocketAddress& GetAddress() const;
		seqid_t GetSequenceId() const;
		void SetSequenceId(seqid_t sequenceId);
		void IncrementSequenceId(seqid_t amount = 1);
		seqid_t GetRemoteSequenceId() const;
		void SetRemoteSequenceId(seqid_t sequenceId);

		uint32_t GetTimeSinceLastPacket() const;
		void ResetTimeSinceLastPacket();
		void UpdateTimeSinceLastPacket(TimeDelta delta);

		uint32_t GetTimeSinceLastSentPacket() const;
		void ResetTimeSinceLastSentPacket();
		void UpdateTimeSinceLastSentPacket(TimeDelta delta);

		uint32_t GetAverageRTT() const;
		void UpdateAverageRTT(uint32_t rtt);

		SequenceBuffer& GetReceivedBuffer();
		SequenceBuffer& GetSentBuffer();

	};

}