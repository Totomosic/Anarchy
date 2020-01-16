#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	template<typename T>
	struct EntityCommand
	{
	public:
		entityid_t NetworkId;
		T Data;
	};

	template<typename T>
	inline void Serialize(OutputMemoryStream& stream, const EntityCommand<T>& command)
	{
		Serialize(stream, command.NetworkId);
		Serialize(stream, command.Data);
	}

	template<typename T>
	inline void Deserialize(InputMemoryStream& stream, EntityCommand<T>& command)
	{
		Deserialize(stream, command.NetworkId);
		Deserialize(stream, command.Data);
	}

	// =======================================================================================
	// ENTITY MOVE COMMAND
	// =======================================================================================

	struct TileMovement
	{
	public:
		static constexpr MessageType Type = MessageType::EntityMoveCommand;

	public:
		Vector2i Movement;
	};

	inline void Serialize(OutputMemoryStream& stream, const TileMovement& command)
	{
		Serialize(stream, command.Movement);
	}

	inline void Deserialize(InputMemoryStream& stream, TileMovement& command)
	{
		Deserialize(stream, command.Movement);
	}

}