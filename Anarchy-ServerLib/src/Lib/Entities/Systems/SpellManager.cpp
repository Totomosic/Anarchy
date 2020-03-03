#include "SpellManager.h"
#include "ServerLib.h"
#include "../Components/NetworkId.h"

namespace Anarchy
{
	SpellManager::SpellManager(ActionQueue* actions)
		: m_Actions(actions)
	{
	}

	void SpellManager::Update(EntityManager& entities, TimeDelta delta)
	{
		for (const EntityHandle& entity : entities.GetEntitiesWith<CCastingSpell>())
		{
			ComponentHandle casting = entity.GetComponent<CCastingSpell>();
			casting->RemainingCastingTimeSeconds -= delta.Seconds();
			if (casting->RemainingCastingTimeSeconds <= 0.0f)
			{
				// Cast spell
				entityid_t networkId = InvalidNetworkId;
				if (entity.HasComponent<CNetworkId>())
				{
					networkId = entity.GetComponent<CNetworkId>()->Id;
				}
				CastSpellAction action;
				action.SpellId = casting->SpellId;
				action.CasterNetworkId = networkId;
				action.SpellData = std::move(casting->SpellData);
				m_Actions->PushAction(networkId, action);
				entity.Remove<CCastingSpell>();
			}
		}
	}

}
