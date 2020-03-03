#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	struct CCastingSpell
	{
	public:
		spellid_t SpellId;
		float RemainingCastingTimeSeconds;
		OutputMemoryStream SpellData;
	};

}