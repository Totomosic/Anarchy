#pragma once
#include "ServerLib.h"
#include "Lib/Data/SpellTypes.h"

namespace Anarchy
{

	struct CCastingSpell
	{
	public:
		SpellType SpellId;
		float RemainingCastingTimeSeconds;
		OutputMemoryStream SpellData;
	};

}