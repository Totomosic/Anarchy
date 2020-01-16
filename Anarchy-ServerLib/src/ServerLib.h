#pragma once
#include "Utils.h"
#include "Utils/Serialization.h"
#include "Utils/Deserialization.h"
#include "Utils/DatabaseConnection.h"
#include "Lib/MessageType.h"

namespace Anarchy
{

	using seqid_t = uint16_t;
	using prefab_t = int;
	using entityid_t = uint32_t;
	using connid_t = uint16_t;
	static constexpr connid_t InvalidConnectionId = (connid_t)-1;

	inline bool IsSeqIdGreater(seqid_t s1, seqid_t s2)
	{
		constexpr seqid_t halfMax = std::numeric_limits<seqid_t>::max() / 2;
		return ((s1 > s2) && (s1 - s2 <= halfMax)) || ((s1 < s2) && (s2 - s1 > halfMax));
	}

	template<typename T>
	struct NetworkMessage
	{
	public:
		seqid_t SequenceId = 0;
		T Message;
	};

	template<typename T>
	inline void Serialize(OutputMemoryStream& stream, const NetworkMessage<T>& message)
	{
		Serialize(stream, message.SequenceId);
		Serialize(stream, message.Message);
	}

	template<typename T>
	inline void Deserialize(InputMemoryStream& stream, NetworkMessage<T>& message)
	{
		Deserialize(stream, message.SequenceId);
		Deserialize(stream, message.Message);
	}

	template<typename T>
	struct ServerNetworkMessage
	{
	public:
		connid_t ConnectionId = InvalidConnectionId;
		seqid_t SequenceId;
		T Message;
	};

	template<typename T>
	inline void Serialize(OutputMemoryStream& stream, const ServerNetworkMessage<T>& message)
	{
		Serialize(stream, message.ConnectionId);
		Serialize(stream, message.SequenceId);
		Serialize(stream, message.Message);
	}

	template<typename T>
	inline void Deserialize(InputMemoryStream& stream, ServerNetworkMessage<T>& message)
	{
		Deserialize(stream, message.ConnectionId);
		Deserialize(stream, message.SequenceId);
		Deserialize(stream, message.Message);
	}

}