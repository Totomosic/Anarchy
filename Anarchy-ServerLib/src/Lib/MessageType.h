#pragma once
#include <cstdint>

namespace Anarchy
{

	enum class MessageType : uint16_t
	{
		None,
		ConnectRequest,
		ConnectResponse,
		DisconnectRequest,
		DisconnectResponse,
		ForceDisconnect,

		CreateCharacterRequest,
		CreateCharacterResponse
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(MessageType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(MessageType);

}