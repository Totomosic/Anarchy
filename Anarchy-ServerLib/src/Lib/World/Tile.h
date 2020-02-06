#pragma once
namespace Anarchy
{

	enum class TileType
	{
		None,
		Grass
	};

	struct Tile
	{
	public:
		TileType Type;
	};

}