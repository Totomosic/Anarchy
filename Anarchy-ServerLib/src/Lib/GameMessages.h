#pragma once
#include "ServerLib.h"
#include "GameState.h"

namespace Anarchy
{

	// =======================================================================================
	// KEEP ALIVE
	// =======================================================================================

	struct KeepAlivePacket
	{
	public:
		static constexpr MessageType Type = MessageType::KeepAlive;

	public:
		seqid_t AckSeqId;
		uint32_t AckBitset;
	};

	inline void Serialize(OutputMemoryStream& stream, const KeepAlivePacket& request)
	{
		Serialize(stream, request.AckSeqId);
		Serialize(stream, request.AckBitset);
	}

	inline void Deserialize(InputMemoryStream& stream, KeepAlivePacket& request)
	{
		Deserialize(stream, request.AckSeqId);
		Deserialize(stream, request.AckBitset);
	}

	// =======================================================================================
	// CREATE CHARACTER
	// =======================================================================================

	struct CreateCharacterRequest
	{
	public:
		static constexpr MessageType Type = MessageType::CreateCharacterRequest;

	};

	inline void Serialize(OutputMemoryStream& stream, const CreateCharacterRequest& request)
	{
	}

	inline void Deserialize(InputMemoryStream& stream, CreateCharacterRequest& request)
	{
	}

	struct CreateCharacterResponse
	{
	public:
		static constexpr MessageType Type = MessageType::CreateCharacterResponse;
	public:
		EntityData Data;
	};

	inline void Serialize(OutputMemoryStream& stream, const CreateCharacterResponse& response)
	{
		Serialize(stream, response.Data);
	}

	inline void Deserialize(InputMemoryStream& stream, CreateCharacterResponse& response)
	{
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
		int DimensionId = 0;
	};

	inline void Serialize(OutputMemoryStream& stream, const GetEntitiesRequest& request)
	{
		Serialize(stream, request.DimensionId);
	}

	inline void Deserialize(InputMemoryStream& stream, GetEntitiesRequest& request)
	{
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
	// SPAWN ENTITIES
	// =======================================================================================

	struct SpawnEntitiesRequest
	{
	public:
		static constexpr MessageType Type = MessageType::SpawnEntities;

	public:
		std::vector<EntityData> Entities;
	};

	inline void Serialize(OutputMemoryStream& stream, const SpawnEntitiesRequest& request)
	{
		Serialize(stream, request.Entities);
	}

	inline void Deserialize(InputMemoryStream& stream, SpawnEntitiesRequest& request)
	{
		Deserialize(stream, request.Entities);
	}

	// =======================================================================================
	// DESTROY ENTITIES
	// =======================================================================================

	struct DestroyEntitiesRequest
	{
	public:
		static constexpr MessageType Type = MessageType::DestroyEntities;

	public:
		std::vector<entityid_t> Entities;
	};

	inline void Serialize(OutputMemoryStream& stream, const DestroyEntitiesRequest& request)
	{
		Serialize(stream, request.Entities);
	}

	inline void Deserialize(InputMemoryStream& stream, DestroyEntitiesRequest& request)
	{
		Deserialize(stream, request.Entities);
	}

	// =======================================================================================
	// UPDATE ENTITIES
	// =======================================================================================

	struct UpdateEntitiesRequest
	{
	public:
		static constexpr MessageType Type = MessageType::UpdateEntities;

	public:
		std::vector<EntityDelta> Updates;
	};

	inline void Serialize(OutputMemoryStream& stream, const UpdateEntitiesRequest& request)
	{
		Serialize(stream, request.Updates);
	}

	inline void Deserialize(InputMemoryStream& stream, UpdateEntitiesRequest& request)
	{
		Deserialize(stream, request.Updates);
	}

}