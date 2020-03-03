#pragma once
#include "Engine/Scene/Systems/System.h"
#include "../Components/CastingSpell.h"
#include "../ActionQueue.h"

namespace Anarchy
{

	class SpellManager : public System<SpellManager>
	{
	private:
		ActionQueue* m_Actions;
	
	public:
		SpellManager(ActionQueue* actions);

		void Update(EntityManager& entities, TimeDelta delta) override;
	};

}