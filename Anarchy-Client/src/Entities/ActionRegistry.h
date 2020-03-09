#pragma once
#include "Lib/Entities/EntityActions.h"
#include "ClientEntityCollection.h"

namespace Anarchy
{

	class ActionRegistry
	{
	public:
		ActionRegistry();

		bool ApplyAction(const GenericAction& action) const;
		void ApplyActions(const std::vector<GenericAction>& actions) const;
		
		template<typename T>
		bool ApplyAction(entityid_t networkId, const T& action) const
		{
			return ApplyActionInternal(networkId, action);
		}

	private:
		template<typename T>
		T CreateAction(const GenericAction& action) const
		{
			T result;
			InputMemoryStream stream = InputMemoryStream::FromStream(*action.ActionData);
			Deserialize(stream, result);
			return result;
		}

		ClientEntityCollection& GetEntities() const;
		EntityHandle GetEntity(entityid_t networkId) const;
		bool ApplyActionInternal(entityid_t networkId, const TileMovementAction& action) const;
		bool ApplyActionInternal(entityid_t networkId, const ChannelSpellAction& action) const;
		bool ApplyActionInternal(entityid_t networkId, const CastSpellAction& action) const;
	};

}