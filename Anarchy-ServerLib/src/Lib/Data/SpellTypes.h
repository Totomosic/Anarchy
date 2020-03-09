#pragma once
#include "Utils/Deserialization.h"
#include "Utils/Serialization.h"
#include "ServerLib.h"

namespace Anarchy
{

	enum class SpellType : spellid_t
	{
		None,
		Fireball
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(SpellType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(SpellType);

	// ==========================================================================================================================================
	// FIREBALL
	// ==========================================================================================================================================

	struct FireballCastData
	{
	public:
		static constexpr SpellType Type = SpellType::Fireball;
	public:
		Vector2i Target;
	};

	inline void Serialize(OutputMemoryStream& stream, const FireballCastData& data)
	{
		Serialize(stream, data.Target);
	}

	inline void Deserialize(InputMemoryStream& stream, FireballCastData& data)
	{
		Deserialize(stream, data.Target);
	}

	struct FireballSpellData
	{
	public:
		static constexpr SpellType Type = SpellType::Fireball;
	public:
		Vector2i Target;
		Vector2i ExplosionSize;
		float TimeToExplode;
		float Damage;
	};

	inline void Serialize(OutputMemoryStream& stream, const FireballSpellData& data)
	{
		Serialize(stream, data.Target);
		Serialize(stream, data.ExplosionSize);
		Serialize(stream, data.TimeToExplode);
		Serialize(stream, data.Damage);
	}

	inline void Deserialize(InputMemoryStream& stream, FireballSpellData& data)
	{
		Deserialize(stream, data.Target);
		Deserialize(stream, data.ExplosionSize);
		Deserialize(stream, data.TimeToExplode);
		Deserialize(stream, data.Damage);
	}

}