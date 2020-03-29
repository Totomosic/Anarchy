#pragma once
#include "Lib/Data/SpellTypes.h"

namespace Anarchy
{

	struct ESpellCast
	{
	public:
		SpellType Type;
		entityid_t CasterId;
		InputMemoryStream CastData;
	};

}