#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	enum class ActionType : uint8_t
	{
		EntityMove,
		ChannelSpell,
		CastSpell,
		InterruptSpell
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
		Serialize(stream, *command.ActionData);
	}

	inline void Deserialize(InputMemoryStream& stream, GenericAction& command)
	{
		Deserialize(stream, command.Action);
		Deserialize(stream, command.ActionId);
		Deserialize(stream, command.NetworkId);
		command.ActionData = std::make_shared<OutputMemoryStream>();
		Deserialize(stream, *command.ActionData);
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

	// =======================================================================================
	// CHANNEL SPELL
	// =======================================================================================

	struct ChannelSpellAction
	{
	public:
		static constexpr ActionType Type = ActionType::ChannelSpell;
	public:
		spellid_t SpellId;
		float ChannelTimeSeconds;
		OutputMemoryStream SpellData;
	};

	inline void Serialize(OutputMemoryStream& stream, const ChannelSpellAction& action)
	{
		Serialize(stream, action.SpellId);
		Serialize(stream, action.ChannelTimeSeconds);
		Serialize(stream, action.SpellData);
	}

	inline void Deserialize(InputMemoryStream& stream, ChannelSpellAction& action)
	{
		Deserialize(stream, action.SpellId);
		Deserialize(stream, action.ChannelTimeSeconds);
		Deserialize(stream, action.SpellData);
	}

	// =======================================================================================
	// CAST SPELL
	// =======================================================================================

	struct CastSpellAction
	{
	public:
		static constexpr ActionType Type = ActionType::CastSpell;
	public:
		entityid_t CasterNetworkId;
		spellid_t SpellId;
		OutputMemoryStream SpellData;
	};

	inline void Serialize(OutputMemoryStream& stream, const CastSpellAction& action)
	{
		Serialize(stream, action.CasterNetworkId);
		Serialize(stream, action.SpellId);
		Serialize(stream, action.SpellData);
	}

	inline void Deserialize(InputMemoryStream& stream, CastSpellAction& action)
	{
		Deserialize(stream, action.CasterNetworkId);
		Deserialize(stream, action.SpellId);
		Deserialize(stream, action.SpellData);
	}

}