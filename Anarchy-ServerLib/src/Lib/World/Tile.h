#pragma once
#include <cstdint>

namespace Anarchy
{

	enum class TileType : uint16_t
	{
		None,
		Grass,
		Sand,
		Water,
		Stone
	};

	struct Tile
	{
	public:
		TileType Type;
	};

}