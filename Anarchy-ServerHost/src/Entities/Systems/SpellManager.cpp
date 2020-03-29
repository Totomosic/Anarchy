#include "serverpch.h"
#include "SpellManager.h"
#include "ServerLib.h"
#include "Core/Time/Time.h"

#include "Lib/Entities/Components/NetworkId.h"
#include "Lib/Entities/Components/TilePosition.h"

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
				m_Actions->PushAction(networkId, CastSpell(networkId, entity, *casting));
				entity.Remove<CCastingSpell>();
			}
		}
	}

	CastSpellAction SpellManager::CastSpell(entityid_t networkId, const EntityHandle& entity, const CCastingSpell& spell)
	{
		switch (spell.SpellId)
		{
		case SpellType::Fireball:
			return CastSpellInternal(networkId, entity, CreateCastData<FireballCastData>(spell.SpellData));
		default:
			break;
		}
		return {};
	}

	CastSpellAction SpellManager::CastSpellInternal(entityid_t networkId, const EntityHandle& entity, const FireballCastData& data)
	{
		FireballSpellData spellData;
		spellData.Target = data.Target;
		spellData.ExplosionSize = { 5, 5 };
		spellData.Speed = 20;
		spellData.Damage = Random::NextInt(8, 48);

		CastSpellAction action;
		action.SpellId = SpellType::Fireball;
		action.CasterNetworkId = networkId;
		Serialize(action.SpellData, spellData);

		Vector2f origin = entity.GetComponent<CTilePosition>()->Position;
		Vector2f toDestination = spellData.Target - origin;
		float length = toDestination.Length();
		float timeToExplode = length / spellData.Speed;

		Time::Get().RenderingTimeline().AddFunction(timeToExplode, [spellData]()
			{
				BLT_INFO("EXPLOSION AT {}", spellData.Target);
			});

		return action;
	}

}
