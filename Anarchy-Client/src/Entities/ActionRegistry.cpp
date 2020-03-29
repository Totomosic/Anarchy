#include "clientpch.h"
#include "ActionRegistry.h"
#include "ClientState.h"

#include "Entities/Components/TileMotion.h"
#include "Lib/Entities/Components/TilePosition.h"
#include "Lib/Entities/Components/CastingSpell.h"

#include "Events/SpellCast.h"

namespace Anarchy
{

	ActionRegistry::ActionRegistry()
	{
	}

	bool ActionRegistry::ApplyAction(const GenericAction& action) const
	{
		switch (action.Action)
		{
		case ActionType::EntityMove:
			return ApplyActionInternal(action.NetworkId, CreateAction<TileMovementAction>(action));
		case ActionType::ChannelSpell:
			return ApplyActionInternal(action.NetworkId, CreateAction<ChannelSpellAction>(action));
		case ActionType::CastSpell:
			return ApplyActionInternal(action.NetworkId, CreateAction<CastSpellAction>(action));
		default:
			break;
		}
		return false;
	}

	void ActionRegistry::ApplyActions(const std::vector<GenericAction>& actions) const
	{
		for (const GenericAction& action : actions)
		{
			ApplyAction(action);
		}
	}

	ClientEntityCollection& ActionRegistry::GetEntities() const
	{
		return ClientState::Get().GetEntities();
	}

	EntityHandle ActionRegistry::GetEntity(entityid_t networkId) const
	{
		return GetEntities().GetEntityByNetworkId(networkId);
	}

	bool ActionRegistry::ApplyActionInternal(entityid_t networkId, const TileMovementAction& action) const
	{
		EntityHandle entity = GetEntity(networkId);
		if (entity)
		{
			ComponentHandle tilePosition = entity.GetComponent<CTilePosition>();
			if (tilePosition)
			{
				Tilemap& tilemap = ClientState::Get().GetTilemap();
				Vector2i nextPosition = tilePosition->Position + action.Movement;
				if (tilemap.GetTile(nextPosition.x, nextPosition.y) != TileType::Water)
				{
					tilePosition->Position = nextPosition;
					if (entity.HasComponent<CTileMotion>())
					{
						ComponentHandle motion = entity.GetComponent<CTileMotion>();
						motion->Movement += action.Movement;
						motion->Speed = action.Speed;
					}
					else
					{
						CTileMotion motion;
						motion.Movement = action.Movement;
						motion.Speed = action.Speed;
						entity.Assign<CTileMotion>(motion);
					}
					return true;
				}
				return false;
			}
		}
		return false;
	}

	bool ActionRegistry::ApplyActionInternal(entityid_t networkId, const ChannelSpellAction& action) const
	{
		EntityHandle entity = GetEntity(networkId);
		if (entity)
		{
			if (!entity.HasComponent<CCastingSpell>())
			{
				CCastingSpell spell;
				spell.SpellId = action.SpellId;
				spell.RemainingCastingTimeSeconds = action.ChannelTimeSeconds;
				spell.SpellData = action.SpellData.Clone();
				entity.Assign<CCastingSpell>(std::move(spell));
				return true;
			}
		}
		return false;
	}

	bool ActionRegistry::ApplyActionInternal(entityid_t networkId, const CastSpellAction& action) const
	{
		EntityHandle entity = GetEntity(networkId);
		if (entity)
		{
			if (entity.HasComponent<CCastingSpell>())
			{
				ComponentHandle casting = entity.GetComponent<CCastingSpell>();
				if (casting->SpellId == action.SpellId)
				{
					EventManager::Get().Bus().Emit(ESpellCast{ action.SpellId, action.CasterNetworkId, InputMemoryStream::FromStream(action.SpellData) });
					entity.Remove<CCastingSpell>();
				}
			}
		}
		return true;
	}

}
