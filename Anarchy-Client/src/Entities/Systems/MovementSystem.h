#pragma once
#include "Engine/Scene/Systems/System.h"
#include "../Components/PlayerController.h"
#include "Lib/Entities/ActionQueue.h"

namespace Anarchy
{

	class MovementSystem : public System<MovementSystem>
	{
	public:
		void Update(EntityManager& entities, TimeDelta delta) override;
	};

}