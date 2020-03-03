#include "clientpch.h"
#include "PlayerControlSystem.h"
#include "Lib/Entities/ActionExecutor.h"
#include "ClientState.h"

#include "Lib/Entities/Components/NetworkId.h"
#include "Lib/Entities/Components/TilePosition.h"
#include "Lib/Entities/Components/CastingSpell.h"
#include "../Components/TileMotion.h"

namespace Anarchy
{

	PlayerControlSystem::PlayerControlSystem(ActionHistory* actionHistory, ActionRegistry* actionRegistry)
		: m_ActionHistory(actionHistory), m_ActionRegistry(actionRegistry)
	{
	}

	void PlayerControlSystem::Update(EntityManager& manager, TimeDelta dt)
	{
		if (m_ActionHistory != nullptr && m_ActionRegistry != nullptr)
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
						TileMovementAction action;
						action.Movement = direction;
						action.Speed = entity.GetComponent<CPlayerController>()->Speed;

						if (m_ActionRegistry->ApplyAction(networkId, action))
						{
							m_ActionHistory->PushAction(networkId, action);
						}
					}
				}
				if (!entity.HasComponent<CCastingSpell>())
				{
					if (Input::Get().KeyDown(Keycode::Q))
					{
						entityid_t networkId = entity.GetComponent<CNetworkId>()->Id;
						ChannelSpellAction action;
						action.ChannelTimeSeconds = 1.0f;
						action.SpellId = 0;

						if (m_ActionRegistry->ApplyAction(networkId, action))
						{
							m_ActionHistory->PushAction(networkId, action);
						}
					}
				}
			}
		}
	}

}