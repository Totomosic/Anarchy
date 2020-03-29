#pragma once
#include "Engine/Scene/Systems/System.h"

namespace Anarchy
{

	class PhysicsSystem : public Bolt::System<PhysicsSystem>
	{
	public:
		virtual void Update(Bolt::EntityManager& entities, Bolt::TimeDelta dt) override;
	};

}