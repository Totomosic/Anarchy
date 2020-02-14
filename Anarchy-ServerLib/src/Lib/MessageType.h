#pragma once
#include <cstdint>

namespace Anarchy
{

	enum class MessageType : uint16_t
	{
		None,
		ChunkSlice,
		ChunkAck,

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

		GetTilemapRequest,
		GetTilemapResponse,

		InputCommand
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(MessageType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(MessageType);

}