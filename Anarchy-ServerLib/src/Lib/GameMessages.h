#pragma once
#include "ServerLib.h"
#include "GameState.h"

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

	struct CreateCharacterResponse
	{
	public:
		static constexpr MessageType Type = MessageType::CreateCharacterResponse;
	public:
		bool Success;
		EntityData Data;
	};

	inline void Serialize(OutputMemoryStream& stream, const CreateCharacterResponse& response)
	{
		Serialize(stream, response.Success);
		Serialize(stream, response.Data);
	}

	inline void Deserialize(InputMemoryStream& stream, CreateCharacterResponse& response)
	{
		Deserialize(stream, response.Success);
		Deserialize(stream, response.Data);
	}

	// =======================================================================================
	// GET ENTITIES
	// =======================================================================================

	struct GetEntitiesRequest
	{
	public:
		static constexpr MessageType Type = MessageType::GetEntitiesRequest;

	public:
		connid_t ConnectionId;
		int DimensionId;
	};

	inline void Serialize(OutputMemoryStream& stream, const GetEntitiesRequest& request)
	{
		Serialize(stream, request.ConnectionId);
		Serialize(stream, request.DimensionId);
	}

	inline void Deserialize(InputMemoryStream& stream, GetEntitiesRequest& request)
	{
		Deserialize(stream, request.ConnectionId);
		Deserialize(stream, request.DimensionId);
	}

	struct GetEntitiesResponse
	{
	public:
		static constexpr MessageType Type = MessageType::GetEntitiesResponse;

	public:
		std::vector<EntityData> Entities;
	};

	inline void Serialize(OutputMemoryStream& stream, const GetEntitiesResponse& response)
	{
		Serialize(stream, response.Entities);
	}

	inline void Deserialize(InputMemoryStream& stream, GetEntitiesResponse& response)
	{
		Deserialize(stream, response.Entities);
	}

	// =======================================================================================
	// GET TILEMAP
	// =======================================================================================

	struct GetTilemapRequest
	{
	public:
		static constexpr MessageType Type = MessageType::GetTilemapRequest;

	public:
		int DimensionId;
		int HeightLevel;
		Vector2i CenterTile;
		int Width;
		int Height;
	};

	struct GetTilemapResponse
	{
	public:
		static constexpr MessageType Type = MessageType::GetTilemapResponse;

	public:
		bool Success;
		Tilemap Data;
	};

}