#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	enum class ActionType
	{
		EntityMove
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(ActionType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(ActionType);

	struct GenericAction
	{
	public:
		static constexpr MessageType Type = MessageType::InputCommand;
	public:
		ActionType Action;
		std::shared_ptr<OutputMemoryStream> ActionData;
	};

	inline void Serialize(OutputMemoryStream& stream, const GenericAction& command)
	{
		Serialize(stream, command.Action);
		Serialize(stream, command.ActionData->GetRemainingDataSize());
		stream.Write(command.ActionData->GetBufferPtr(), command.ActionData->GetRemainingDataSize());
		command.ActionData->Skip(command.ActionData->GetRemainingDataSize());
	}

	inline void Deserialize(InputMemoryStream& stream, GenericAction& command)
	{
		Deserialize(stream, command.Action);
		size_t size;
		Deserialize(stream, size);
		command.ActionData = std::make_shared<OutputMemoryStream>(size);
		command.ActionData->Write(stream.GetBufferPtr(), size);
		stream.Skip(size);
	}

	template<typename T>
	struct InputAction
	{
	public:
		static constexpr MessageType Type = MessageType::InputCommand;

	public:
		entityid_t NetworkId;
		T Action;
	};

	template<typename T>
	inline void Serialize(OutputMemoryStream& stream, const InputAction<T>& command)
	{
		Serialize(stream, command.NetworkId);
		Serialize(stream, command.Action);
	}

	template<typename T>
	inline void Deserialize(InputMemoryStream& stream, InputAction<T>& command)
	{
		Deserialize(stream, command.NetworkId);
		Deserialize(stream, command.Action);
	}

	// =======================================================================================
	// ENTITY MOVE COMMAND
	// =======================================================================================

	struct TileMovement
	{
	public:
		static constexpr ActionType Type = ActionType::EntityMove;
	public:
		Vector2i Destination;
		float Speed;
	};

	inline void Serialize(OutputMemoryStream& stream, const TileMovement& command)
	{
		Serialize(stream, command.Destination);
		Serialize(stream, command.Speed);
	}

	inline void Deserialize(InputMemoryStream& stream, TileMovement& command)
	{
		Deserialize(stream, command.Destination);
		Deserialize(stream, command.Speed);
	}

}