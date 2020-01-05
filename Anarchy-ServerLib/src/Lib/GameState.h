#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	// =======================================================================================
	// ETNTITY DATA
	// =======================================================================================

	struct EntityData
	{
	public:
		entityid_t NetworkId;
		prefab_t PrefabId;
		int Level;
		int DimensionId;
		int HeightLevel;
		Vector2i TilePosition;
	};

	inline void Serialize(OutputMemoryStream& stream, const EntityData& value)
	{
		Serialize(stream, value.NetworkId);
		Serialize(stream, value.PrefabId);
		Serialize(stream, value.Level);
		Serialize(stream, value.DimensionId);
		Serialize(stream, value.HeightLevel);
		Serialize(stream, value.TilePosition);
	}

	inline void Deserialize(InputMemoryStream& stream, EntityData& value)
	{
		Deserialize(stream, value.NetworkId);
		Deserialize(stream, value.PrefabId);
		Deserialize(stream, value.Level);
		Deserialize(stream, value.DimensionId);
		Deserialize(stream, value.HeightLevel);
		Deserialize(stream, value.TilePosition);
	}

	// =======================================================================================
	// TILEMAP
	// =======================================================================================

	enum class TileType : uint8_t
	{
		None,
		Grass,
		Water
	};

	ANCH_DEFINE_DEFAULT_SERIALIZE(TileType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(TileType);

	struct Tilemap
	{
	public:
		std::vector<TileType> Tiles;
	};

	inline void Serialize(OutputMemoryStream& stream, const Tilemap& value)
	{
		Serialize(stream, value.Tiles);
	}

	inline void Deserialize(InputMemoryStream& stream, Tilemap& value)
	{
		Deserialize(stream, value.Tiles);
	}

}