#pragma once
#include "Utils/Serialization.h"
#include "Utils/Deserialization.h"
#include "ServerLib.h"

namespace Anarchy
{
	
	// =======================================================================================
	// ENTITY DIED
	// =======================================================================================

	struct MEntityDied
	{
	public:
		static constexpr MessageType Type = MessageType::EntityDied;
	public:
		entityid_t NetworkId;
	};

	inline void Serialize(OutputMemoryStream& stream, const MEntityDied& message)
	{
		Serialize(stream, message.NetworkId);
	}

	inline void Deserialize(InputMemoryStream& stream, MEntityDied& message)
	{
		Deserialize(stream, message.NetworkId);
	}

	// =======================================================================================
	// ENTITY RESPAWNED
	// =======================================================================================

	// =======================================================================================
	// ENTITY DAMAGED
	// =======================================================================================

	enum class DamageType : uint8_t
	{
		Unknown,
		Piercing,
		Slashing,
		Bludgeoning,
		Cold,
		Lightning,
		Fire,
		Force,
		Acid,
		Necrotic,
		Poison,
		Psychic,
		Radiant,
		Thunder
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(DamageType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(DamageType);

	struct DamageSource
	{
	public:
		float Damage;
		DamageType Type;
	};

	inline void Serialize(OutputMemoryStream& stream, const DamageSource& value)
	{
		Serialize(stream, value.Damage);
		Serialize(stream, value.Type);
	}

	inline void Deserialize(InputMemoryStream& stream, DamageSource& value)
	{
		Deserialize(stream, value.Damage);
		Deserialize(stream, value.Type);
	}

	struct MEntityDamaged
	{
	public:
		static constexpr MessageType Type = MessageType::EntityDamaged;
	public:
		entityid_t NetworkId;
		std::vector<DamageSource> DamageSources;
	};

	inline void Serialize(OutputMemoryStream& stream, const MEntityDamaged& message)
	{
		Serialize(stream, message.NetworkId);
		Serialize(stream, message.DamageSources);
	}

	inline void Deserialize(InputMemoryStream& stream, MEntityDamaged& message)
	{
		Deserialize(stream, message.NetworkId);
		Deserialize(stream, message.DamageSources);
	}

	// =======================================================================================
	// SET CONTROLLED ENTITY
	// =======================================================================================

	struct MSetControlledEntity
	{
	public:
		static constexpr MessageType Type = MessageType::SetControlledEntity;
	public:
		entityid_t NetworkId;
	};

	inline void Serialize(OutputMemoryStream& stream, const MSetControlledEntity& message)
	{
		Serialize(stream, message.NetworkId);
	}

	inline void Deserialize(InputMemoryStream& stream, MSetControlledEntity& message)
	{
		Deserialize(stream, message.NetworkId);
	}

}