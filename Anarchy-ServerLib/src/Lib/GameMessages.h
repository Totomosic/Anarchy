#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	// =======================================================================================
	// CREATE CHARACTER
	// =======================================================================================

	struct CreateCharacterRequest
	{
	public:
		static constexpr MessageType Type = MessageType::CreateCharacterRequest;
	public:
		connid_t ConnectionId;
	};

	inline void Serialize(OutputMemoryStream& stream, const CreateCharacterRequest& request)
	{
		Serialize(stream, request.ConnectionId);
	}

	inline void Deserialize(InputMemoryStream& stream, CreateCharacterRequest& request)
	{
		Deserialize(stream, request.ConnectionId);
	}

	struct CharacterData
	{
	public:
		entityid_t EntityId;
		prefab_t PrefabId;
		int Level;
		blt::string Name;
		int DimensionId;
		Vector2i TilePosition;
	};

	inline void Serialize(OutputMemoryStream& stream, const CharacterData& value)
	{
		Serialize(stream, value.EntityId);
		Serialize(stream, value.PrefabId);
		Serialize(stream, value.Level);
		Serialize(stream, value.Name);
		Serialize(stream, value.DimensionId);
		Serialize(stream, value.TilePosition);
	}

	inline void Deserialize(InputMemoryStream& stream, CharacterData& value)
	{
		Deserialize(stream, value.EntityId);
		Deserialize(stream, value.PrefabId);
		Deserialize(stream, value.Level);
		Deserialize(stream, value.Name);
		Deserialize(stream, value.DimensionId);
		Deserialize(stream, value.TilePosition);
	}

	struct CreateCharacterResponse
	{
	public:
		static constexpr MessageType Type = MessageType::CreateCharacterResponse;
	public:
		bool Success;
		CharacterData Data;
	};

	inline void Serialize(OutputMemoryStream& stream, const CreateCharacterResponse& request)
	{
		Serialize(stream, request.Success);
		Serialize(stream, request.Data);
	}

	inline void Deserialize(InputMemoryStream& stream, CreateCharacterResponse& request)
	{
		Deserialize(stream, request.Success);
		Deserialize(stream, request.Data);
	}

}