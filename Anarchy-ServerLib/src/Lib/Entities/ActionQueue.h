#pragma once
#include "EntityActions.h"

namespace Anarchy
{

	class ActionQueue
	{
	private:
		std::vector<GenericAction> m_Actions;

	public:
		ActionQueue();

		const std::vector<GenericAction>& GetAllActions() const;

		void PushAction(const GenericAction& action);
		void Clear();

		template<typename T>
		void PushAction(entityid_t entityId, const T& action)
		{
			GenericAction result;
			result.ActionData = std::make_shared<OutputMemoryStream>(sizeof(T));
			result.NetworkId = entityId;
			result.Action = T::Type;
			Serialize(*result.ActionData, action);
			PushAction(result);
		}
	};

}