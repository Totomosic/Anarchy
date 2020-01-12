#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	// =======================================================================================
	// ENTITY MOVE COMMAND
	// =======================================================================================

	struct EntityMoveCommand
	{
	public:
		static constexpr MessageType Type = MessageType::EntityMoveCommand;

	public:
		entityid_t NetworkId;
		Vector2i Movement;
	};

	inline void Serialize(OutputMemoryStream& stream, const EntityMoveCommand& command)
	{
		Serialize(stream, command.NetworkId);
		Serialize(stream, command.Movement);
	}

	inline void Deserialize(InputMemoryStream& stream, EntityMoveCommand& command)
	{
		Deserialize(stream, command.NetworkId);
		Deserialize(stream, command.Movement);
	}

}