#include "clientpch.h"
#include "ActionRegistry.h"
#include "ClientState.h"

#include "Entities/Components/TileMotion.h"
#include "Lib/Entities/Components/TilePosition.h"

namespace Anarchy
{

	ActionRegistry::ActionRegistry()
	{
	}

	void ActionRegistry::ApplyAction(const GenericAction& action) const
	{
		switch (action.Action)
		{
		case ActionType::EntityMove:
			ApplyAction(action.NetworkId, CreateAction<TileMovementAction>(action));
			break;
		default:
			break;
		}
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

	void ActionRegistry::ApplyActionInternal(entityid_t networkId, const TileMovementAction& action) const
	{
		ClientEntityCollection& entities = GetEntities();
		EntityHandle entity = entities.GetEntityByNetworkId(networkId);
		if (entity)
		{
			ComponentHandle tilePosition = entity.GetComponent<CTilePosition>();
			if (tilePosition)
			{
				tilePosition->Position += action.Movement;
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
			}
		}
	}

}
