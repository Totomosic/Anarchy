#pragma once
#include "Lib/Entities/EntityActions.h"
#include "ServerEntityCollection.h"

namespace Anarchy
{

	class ActionRegistry
	{
	public:
		ActionRegistry();

		bool ApplyAction(const GenericAction& action) const;
		std::vector<GenericAction> ApplyActions(const std::vector<GenericAction>& actions) const;

	private:
		template<typename T>
		T CreateAction(const GenericAction& action) const
		{
			T result;
			InputMemoryStream stream = InputMemoryStream::FromStream(*action.ActionData);
			Deserialize(stream, result);
			return result;
		}

		ServerEntityCollection& GetEntities() const;
		EntityHandle GetEntity(entityid_t networkId) const;
		bool ApplyActionInternal(entityid_t networkId, const TileMovementAction& action) const;
		bool ApplyActionInternal(entityid_t networkId, const ChannelSpellAction& action) const;
	};

}