#include "ActionExecutor.h"
#include "Components/CastingSpell.h"

namespace Anarchy
{

	ActionExecutor::ActionExecutor()
	{
	}

	EntityState ActionExecutor::ApplyAction(const EntityState& state, const GenericAction& action) const
	{
		EntityState finalState = state;
		InputMemoryStream data = InputMemoryStream::FromStream(*action.ActionData);
		switch (action.Action)
		{
		case ActionType::EntityMove:
			ApplyActionInternal(finalState, CreateAction<TileMovementAction>(data));
			break;
		case ActionType::ChannelSpell:
			ApplyActionInternal(finalState, CreateAction<ChannelSpellAction>(data));
			break;
		default:
			break;
		}
		return finalState;
	}

	EntityState ActionExecutor::ApplyActions(const EntityState& state, const std::vector<GenericAction>& actions) const
	{
		EntityState finalState = state;
		for (const GenericAction& action : actions)
		{
			InputMemoryStream data = InputMemoryStream::FromStream(*action.ActionData);
			switch (action.Action)
			{
			case ActionType::EntityMove:
				ApplyActionInternal(finalState, CreateAction<TileMovementAction>(data));
				break;
			case ActionType::ChannelSpell:
				ApplyActionInternal(finalState, CreateAction<ChannelSpellAction>(data));
				break;
			default:
				break;
			}
		}
		return finalState;
	}

	void ActionExecutor::ApplyActionInternal(EntityState& state, const TileMovementAction& action) const
	{
		state.TilePosition += action.Movement;
	}

	void ActionExecutor::ApplyActionInternal(EntityState& state, const ChannelSpellAction& action) const
	{
	}

}
