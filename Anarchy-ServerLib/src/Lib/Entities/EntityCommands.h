#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	enum class CommandType
	{
		EntityMove
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(CommandType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(CommandType);

	struct GenericCommand
	{
	public:
		static constexpr MessageType Type = MessageType::InputCommand;
	public:
		Anarchy::CommandType CommandType;
		std::shared_ptr<OutputMemoryStream> CommandData;
	};

	inline void Serialize(OutputMemoryStream& stream, const GenericCommand& command)
	{
		Serialize(stream, command.CommandType);
		Serialize(stream, command.CommandData->GetRemainingDataSize());
		stream.Write(command.CommandData->GetBufferPtr(), command.CommandData->GetRemainingDataSize());
	}

	inline void Deserialize(InputMemoryStream& stream, GenericCommand& command)
	{
		Deserialize(stream, command.CommandType);
		uint32_t size;
		Deserialize(stream, size);
		command.CommandData = std::make_shared<OutputMemoryStream>(size);
		command.CommandData->Write(stream.GetBufferPtr(), size);
	}

	template<typename T>
	struct InputCommand
	{
	public:
		static constexpr MessageType Type = MessageType::InputCommand;

	public:
		entityid_t NetworkId;
		T Command;
	};

	template<typename T>
	inline void Serialize(OutputMemoryStream& stream, const InputCommand<T>& command)
	{
		Serialize(stream, command.NetworkId);
		Serialize(stream, command.Command);
	}

	template<typename T>
	inline void Deserialize(InputMemoryStream& stream, InputCommand<T>& command)
	{
		Deserialize(stream, command.NetworkId);
		Deserialize(stream, command.Command);
	}

	// =======================================================================================
	// ENTITY MOVE COMMAND
	// =======================================================================================

	struct TileMovement
	{
	public:
		static constexpr CommandType Type = CommandType::EntityMove;
	public:
		Vector2i Movement;
		float Speed;
	};

	inline void Serialize(OutputMemoryStream& stream, const TileMovement& command)
	{
		Serialize(stream, command.Movement);
		Serialize(stream, command.Speed);
	}

	inline void Deserialize(InputMemoryStream& stream, TileMovement& command)
	{
		Deserialize(stream, command.Movement);
		Deserialize(stream, command.Speed);
	}

}