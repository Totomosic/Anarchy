#include "clientpch.h"
#include "PlayerControlSystem.h"

#include "Lib/Entities/Components/NetworkId.h"
#include "Lib/Entities/Components/TilePosition.h"
#include "../Components/TileMotion.h"

namespace Anarchy
{

	PlayerControlSystem::PlayerControlSystem(ActionBuffer* actionBuffer)
		: m_ActionBuffer(actionBuffer)
	{
	}

	void PlayerControlSystem::Update(EntityManager& manager, TimeDelta dt)
	{
		if (m_ActionBuffer != nullptr)
		{
			for (EntityHandle entity : manager.GetEntitiesWith<CPlayerController, CTilePosition, CNetworkId>())
			{
				if (!entity.HasComponent<CTileMotion>())
				{
					Vector2i direction = { 0, 0 };
					if (Input::Get().KeyDown(Keycode::W))
					{
						direction = { 0, 1 };
					}
					else if (Input::Get().KeyDown(Keycode::S))
					{
						direction = { 0, -1 };
					}
					else if (Input::Get().KeyDown(Keycode::D))
					{
						direction = { 1, 0 };
					}
					else if (Input::Get().KeyDown(Keycode::A))
					{
						direction = { -1, 0 };
					}

					if (direction.x != 0 || direction.y != 0)
					{
						entityid_t networkId = entity.GetComponent<CNetworkId>()->Id;
						InputAction<TileMovement> action;
						action.NetworkId = networkId;
						action.Action.Destination = entity.GetComponent<CTilePosition>()->Position + direction;
						action.Action.Speed = entity.GetComponent<CPlayerController>()->Speed;
						m_ActionBuffer->PushAction(action);
					}
				}
			}
		}
	}

}