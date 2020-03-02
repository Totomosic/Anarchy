#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	enum class ActionType : uint8_t
	{
		EntityMove = 101
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(ActionType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(ActionType);

	struct GenericAction
	{
	public:
		static constexpr MessageType Type = MessageType::InputCommand;
	public:
		ActionType Action;
		seqid_t ActionId;
		entityid_t NetworkId;
		std::shared_ptr<OutputMemoryStream> ActionData;
	};

	inline void Serialize(OutputMemoryStream& stream, const GenericAction& command)
	{
		Serialize(stream, command.Action);
		Serialize(stream, command.ActionId);
		Serialize(stream, command.NetworkId);
		Serialize(stream, command.ActionData->GetRemainingDataSize());
		stream.Write(command.ActionData->GetBufferPtr(), command.ActionData->GetRemainingDataSize());
	}

	inline void Deserialize(InputMemoryStream& stream, GenericAction& command)
	{
		Deserialize(stream, command.Action);
		Deserialize(stream, command.ActionId);
		Deserialize(stream, command.NetworkId);
		size_t size;
		Deserialize(stream, size);
		command.ActionData = std::make_shared<OutputMemoryStream>(size);
		command.ActionData->WriteFromStream(stream, size);
	}

	// =======================================================================================
	// ENTITY MOVE ACTION
	// =======================================================================================

	struct TileMovementAction
	{
	public:
		static constexpr ActionType Type = ActionType::EntityMove;
	public:
		Vector2i Movement;
		float Speed;
	};

	inline void Serialize(OutputMemoryStream& stream, const TileMovementAction& action)
	{
		Serialize(stream, action.Movement);
		Serialize(stream, action.Speed);
	}

	inline void Deserialize(InputMemoryStream& stream, TileMovementAction& action)
	{
		Deserialize(stream, action.Movement);
		Deserialize(stream, action.Speed);
	}

}