#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	// =======================================================================================
	// ENTITY DATA
	// =======================================================================================

	struct EntityData
	{
	public:
		entityid_t NetworkId = 0;
		prefab_t PrefabId = 0;
		int Level = 1;
		int DimensionId = 0;
		int HeightLevel = 0;
		Vector2i TilePosition;
		std::string Name;

		float MaxHealth;
		float CurrentHealth;
		float Shield;
	};

	inline void Serialize(OutputMemoryStream& stream, const EntityData& value)
	{
		Serialize(stream, value.NetworkId);
		Serialize(stream, value.PrefabId);
		Serialize(stream, value.Level);
		Serialize(stream, value.DimensionId);
		Serialize(stream, value.HeightLevel);
		Serialize(stream, value.TilePosition);
		Serialize(stream, value.Name);

		Serialize(stream, value.MaxHealth);
		Serialize(stream, value.CurrentHealth);
		Serialize(stream, value.Shield);
	}

	inline void Deserialize(InputMemoryStream& stream, EntityData& value)
	{
		Deserialize(stream, value.NetworkId);
		Deserialize(stream, value.PrefabId);
		Deserialize(stream, value.Level);
		Deserialize(stream, value.DimensionId);
		Deserialize(stream, value.HeightLevel);
		Deserialize(stream, value.TilePosition);
		Deserialize(stream, value.Name);

		Deserialize(stream, value.MaxHealth);
		Deserialize(stream, value.CurrentHealth);
		Deserialize(stream, value.Shield);
	}

}