#pragma once
#include <cstdint>

namespace Anarchy
{

	enum class MessageType : uint16_t
	{
		None,
		KeepAlive,

		ConnectRequest,
		ConnectResponse,
		DisconnectRequest,
		DisconnectResponse,
		ForceDisconnect,

		CreateCharacterRequest,
		CreateCharacterResponse,

		GetEntitiesRequest,
		GetEntitiesResponse,

		SpawnEntities,
		DestroyEntities,
		UpdateEntities,

		InputCommand
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(MessageType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(MessageType);

}