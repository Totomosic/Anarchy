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
		GetTilemapRequest,
		GetTilemapResponse,

		SpawnEntities,
		DestroyEntities,
		UpdateEntities,

		InputCommand,

		EntityDied,
		EntityDamaged,
		EntityRespawned
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(MessageType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(MessageType);

	enum class MessageCategory : uint8_t
	{
		Any,
		Request,
		Response
	};

	inline MessageCategory GetMessageCategory(MessageType type)
	{
		switch (type)
		{
		case MessageType::ConnectRequest:
			return MessageCategory::Request;
		case MessageType::DisconnectRequest:
			return MessageCategory::Request;
		case MessageType::CreateCharacterRequest:
			return MessageCategory::Request;
		case MessageType::GetEntitiesRequest:
			return MessageCategory::Request;
		case MessageType::GetTilemapRequest:
			return MessageCategory::Request;
		case MessageType::ConnectResponse:
			return MessageCategory::Response;
		case MessageType::DisconnectResponse:
			return MessageCategory::Response;
		case MessageType::CreateCharacterResponse:
			return MessageCategory::Response;
		case MessageType::GetEntitiesResponse:
			return MessageCategory::Response;
		case MessageType::GetTilemapResponse:
			return MessageCategory::Response;
		default:
			return MessageCategory::Any;
		}
		return MessageCategory::Any;
	}

}