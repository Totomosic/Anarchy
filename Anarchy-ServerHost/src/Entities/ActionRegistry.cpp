#include "serverpch.h"
#include "ActionRegistry.h"
#include "ServerState.h"

#include "Lib/Entities/Components/TilePosition.h"
#include "Lib/Entities/Components/CastingSpell.h"

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
		default:
			break;
		}
		return false;
	}

	std::vector<GenericAction> ActionRegistry::ApplyActions(const std::vector<GenericAction>& actions) const
	{
		std::vector<GenericAction> result;
		for (const GenericAction& action : actions)
		{
			if (ApplyAction(action))
			{
				result.push_back(action);
			}
		}
		return result;
	}

	ServerEntityCollection& ActionRegistry::GetEntities() const
	{
		return ServerState::Get().GetEntities();
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
			if (entity.HasComponent<CTilePosition>())
			{
				WorldReader& world = ServerState::Get().GetWorld();
				ComponentHandle position = entity.GetComponent<CTilePosition>();
				Vector2i nextPos = position->Position + action.Movement;
				if (world.GetTile(nextPos.x, nextPos.y) == TileType::Water)
				{
					return false;
				}
				position->Position = nextPos;
				return true;
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

}
