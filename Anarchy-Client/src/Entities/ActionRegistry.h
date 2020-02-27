#pragma once
#include "Lib/Entities/EntityActions.h"
#include "ClientEntityCollection.h"

namespace Anarchy
{

	class ActionRegistry
	{
	public:
		ActionRegistry();

		void ApplyAction(const GenericAction& action) const;
		void ApplyActions(const std::vector<GenericAction>& actions) const;
		
		template<typename T>
		void ApplyAction(entityid_t networkId, const T& action) const
		{
			ApplyActionInternal(networkId, action);
		}

	private:
		template<typename T>
		T CreateAction(const GenericAction& action) const
		{
			T result;
			InputMemoryStream stream(action.ActionData->GetRemainingDataSize());
			memcpy(stream.GetBufferPtr(), action.ActionData->GetBufferPtr(), stream.GetRemainingDataSize());
			Deserialize(stream, result);
			return result;
		}

		ClientEntityCollection& GetEntities() const;
		void ApplyActionInternal(entityid_t networkId, const TileMovementAction& action) const;
	};

}