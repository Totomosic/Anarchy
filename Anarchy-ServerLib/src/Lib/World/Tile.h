#pragma once
#include <cstdint>
#include "Utils/Serialization.h"
#include "Utils/Deserialization.h"

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

	ANCH_DEFINE_DEFAULT_SERIALIZE(TileType);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(TileType);

}