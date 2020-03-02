#pragma once
#include "ServerLib.h"
#include "Lib/Entities/EntityState.h"
#include "Lib/Entities/EntityActions.h"
#include "World/Tile.h"

namespace Anarchy
{

	// =======================================================================================
	// KEEP ALIVE
	// =======================================================================================

	struct KeepAlivePacket
	{
	public:
		static constexpr MessageType Type = MessageType::KeepAlive;
	};

	inline void Serialize(OutputMemoryStream& stream, const KeepAlivePacket& request)
	{
	}

	inline void Deserialize(InputMemoryStream& stream, KeepAlivePacket& request)
	{
	}

	// =======================================================================================
	// FORCE DISCONNECT
	// =======================================================================================

	struct ForceDisconnectMessage
	{
	public:
		static constexpr MessageType Type = MessageType::ForceDisconnect;
	};

	inline void Serialize(OutputMemoryStream& stream, const ForceDisconnectMessage& value)
	{
	}

	inline void Deserialize(InputMemoryStream& stream, ForceDisconnectMessage& value)
	{
	}

	// =======================================================================================
	// SPAWN ENTITIES
	// =======================================================================================

	struct SpawnEntitiesRequest
	{
	public:
		static constexpr MessageType Type = MessageType::SpawnEntities;
	public:
		std::vector<EntityState> Entities;
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

	struct EntityUpdate
	{
	public:
		EntityState FinalState;
		std::vector<GenericAction> Actions;
		std::optional<seqid_t> MaxActionId = {};
	};

	inline void Serialize(OutputMemoryStream& stream, const EntityUpdate& update)
	{
		Serialize(stream, update.FinalState);
		Serialize(stream, update.Actions);
		Serialize(stream, update.MaxActionId);
	}

	inline void Deserialize(InputMemoryStream& stream, EntityUpdate& update)
	{
		Deserialize(stream, update.FinalState);
		Deserialize(stream, update.Actions);
		Deserialize(stream, update.MaxActionId);
	}

	struct UpdateEntitiesRequest
	{
	public:
		static constexpr MessageType Type = MessageType::UpdateEntities;
	public:
		std::vector<EntityUpdate> Updates;
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