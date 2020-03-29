#include "PhysicsSystem.h"
#include "Engine/Scene/Components/Transform.h"
#include "../Components/LinearPath.h"

namespace Anarchy
{

	void PhysicsSystem::Update(Bolt::EntityManager& entities, Bolt::TimeDelta dt)
	{
		for (Bolt::EntityHandle entity : entities.GetEntitiesWith<Bolt::Transform, CLinearPath>())
		{
			Bolt::ComponentHandle transform = entity.GetTransform();
			Bolt::ComponentHandle path = entity.GetComponent<CLinearPath>();

			Bolt::Vector2f current = transform->Position().xy();
			Bolt::Vector2f target = path->Destination;
			Bolt::Vector2f toTarget = target - current;
			float length = toTarget.Length();
			float maxLength = path->Speed * dt.Seconds();
			if (length < maxLength)
			{
				transform->SetLocalXY(path->Destination);
				entity.Remove<CLinearPath>();
			}
			else
			{
				transform->Translate({ toTarget.Normalize() * maxLength, 0.0f });
			}
		}
	}

}
