#pragma once
#include "EntityActions.h"
#include "EntityState.h"

namespace Anarchy
{

	class ActionExecutor
	{
	public:
		ActionExecutor();

		EntityState ApplyAction(const EntityState& state, const GenericAction& action) const;
		EntityState ApplyActions(const EntityState& state, const std::vector<GenericAction>& actions) const;

		template<typename T>
		EntityState ApplyAction(const EntityState& state, const T& action) const
		{
			EntityState s = state;
			ApplyActionInternal(s, action);
			return s;
		}

	private:
		template<typename T>
		T CreateAction(InputMemoryStream& data) const
		{
			T action;
			Deserialize(data, action);
			return action;
		}

		void ApplyActionInternal(EntityState& state, const TileMovementAction& action) const;
	};

}