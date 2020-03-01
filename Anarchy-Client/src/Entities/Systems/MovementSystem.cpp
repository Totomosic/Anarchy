#include "clientpch.h"
#include "MovementSystem.h"

#include "Lib/Entities/Components/TilePosition.h"
#include "../Components/TileMotion.h"

namespace Anarchy
{

	void MovementSystem::Update(EntityManager& entities, TimeDelta delta)
	{
		for (EntityHandle entity : entities.GetEntitiesWith<Transform, CTilePosition, CTileMotion>())
		{
			ComponentHandle transform = entity.GetComponent<Transform>();
			ComponentHandle motion = entity.GetComponent<CTileMotion>();
			Vector2f toDestination = motion->Movement;
			float length = toDestination.Length();
			float movement = std::min(length, motion->Speed * (float)delta.Seconds());
			if (movement == length)
			{
				ComponentHandle tilePosition = entity.GetComponent<CTilePosition>();
				Vector2f currentXY = transform->Position().xy();
				transform->SetLocalXY(tilePosition->Position);
				entity.Remove<CTileMotion>();
			}
			else
			{
				Vector2f deltaMove = toDestination / length * movement;
				transform->Translate({ deltaMove, 0.0f });
				motion->Movement -= deltaMove;
			}
		}
	}

}
