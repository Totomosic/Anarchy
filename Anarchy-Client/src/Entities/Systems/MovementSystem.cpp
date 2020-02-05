#include "clientpch.h"
#include "MovementSystem.h"

#include "Lib/Entities/Components/TilePosition.h"
#include "../Components/TileMotion.h"

namespace Anarchy
{

	void MovementSystem::Update(EntityManager& entities, TimeDelta delta)
	{
		for (EntityHandle entity : entities.GetEntitiesWith<Transform, TilePosition, CTileMotion>())
		{
			ComponentHandle transform = entity.GetComponent<Transform>();
			ComponentHandle motion = entity.GetComponent<CTileMotion>();
			Vector2f toDestination = (Vector2f)motion->Destination - transform->Position().xy();
			float length = toDestination.Length();
			float movement = std::min(length, motion->Speed * (float)delta.Seconds());
			if (movement == length)
			{
				transform->SetLocalXY(motion->Destination);
				entity.Remove<CTileMotion>();
			}
			else
			{
				transform->Translate({ toDestination / length * movement, 0.0f });
			}
		}
	}

}
