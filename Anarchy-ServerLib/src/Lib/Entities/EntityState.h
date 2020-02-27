#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	struct EntityState
	{
	public:
		entityid_t NetworkId;
		prefab_t PrefabId;
		int HeightLevel;
		int DimensionId;
		Vector2i TilePosition;
		Vector2i TileSize;
		int Level;
		std::string Name;
		float CurrentHealth;
		float MaxHealth;
		float CurrentShield;
	};

	inline bool operator==(const EntityState& left, const EntityState& right)
	{
		return left.NetworkId == right.NetworkId && left.PrefabId == right.PrefabId && left.HeightLevel == right.HeightLevel && left.DimensionId == right.DimensionId && left.TilePosition == right.TilePosition
			&& left.TileSize == right.TileSize && left.Level == right.Level && left.Name == right.Name && left.CurrentHealth == right.CurrentHealth && left.MaxHealth == right.MaxHealth && left.CurrentShield == right.CurrentShield;
	}

	inline bool operator!=(const EntityState& left, const EntityState& right)
	{
		return !(left == right);
	}

	inline void Serialize(OutputMemoryStream& stream, const EntityState& state)
	{
		Serialize(stream, state.NetworkId);
		Serialize(stream, state.PrefabId);
		Serialize(stream, state.HeightLevel);
		Serialize(stream, state.DimensionId);
		Serialize(stream, state.TilePosition);
		Serialize(stream, state.TileSize);
		Serialize(stream, state.Level);
		Serialize(stream, state.Name);
		Serialize(stream, state.CurrentHealth);
		Serialize(stream, state.MaxHealth);
		Serialize(stream, state.CurrentShield);
	}

	inline void Deserialize(InputMemoryStream& stream, EntityState& state)
	{
		Deserialize(stream, state.NetworkId);
		Deserialize(stream, state.PrefabId);
		Deserialize(stream, state.HeightLevel);
		Deserialize(stream, state.DimensionId);
		Deserialize(stream, state.TilePosition);
		Deserialize(stream, state.TileSize);
		Deserialize(stream, state.Level);
		Deserialize(stream, state.Name);
		Deserialize(stream, state.CurrentHealth);
		Deserialize(stream, state.MaxHealth);
		Deserialize(stream, state.CurrentShield);
	}

}