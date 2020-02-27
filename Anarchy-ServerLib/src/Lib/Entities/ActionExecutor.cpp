#include "ActionExecutor.h"

namespace Anarchy
{

	ActionExecutor::ActionExecutor()
	{
	}

	EntityState ActionExecutor::ApplyAction(const EntityState& state, const GenericAction& action) const
	{
		EntityState finalState = state;
		InputMemoryStream data(action.ActionData->GetRemainingDataSize());
		memcpy(data.GetBufferPtr(), action.ActionData->GetBufferPtr(), data.GetRemainingDataSize());
		switch (action.Action)
		{
		case ActionType::EntityMove:
			ApplyActionInternal(finalState, CreateAction<TileMovementAction>(data));
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
			InputMemoryStream data(action.ActionData->GetRemainingDataSize());
			memcpy(data.GetBufferPtr(), action.ActionData->GetBufferPtr(), data.GetRemainingDataSize());
			switch (action.Action)
			{
			case ActionType::EntityMove:
				ApplyActionInternal(finalState, CreateAction<TileMovementAction>(data));
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

}
