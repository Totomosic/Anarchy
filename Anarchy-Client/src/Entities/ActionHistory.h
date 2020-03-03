#pragma once
#include "Lib/Entities/EntityActions.h"
#include "Lib/Entities/ActionExecutor.h"

namespace Anarchy
{

	class ActionHistory
	{
	private:
		std::vector<GenericAction> m_Actions;
		seqid_t m_NextActionId;

	public:
		ActionHistory();

		seqid_t GetNextActionId();
		seqid_t PeekNextActionId() const;
		const std::vector<GenericAction>& GetAllActions() const;
		std::vector<GenericAction> GetActionsAfter(seqid_t actionId) const;
		bool ContainsAction(seqid_t actionId) const;

		// Adds an action and sends it over the network
		void PushAction(const GenericAction& action);
		void ClearAllActions();
		void ClearActionsBeforeIncluding(seqid_t actionId);
		void Reset();

		template<typename T>
		void PushAction(entityid_t networkId, const T& action)
		{
			GenericAction a;
			a.Action = T::Type;
			a.ActionId = GetNextActionId();
			a.NetworkId = networkId;
			a.ActionData = std::make_shared<OutputMemoryStream>(sizeof(T));
			Serialize(*a.ActionData, action);
			PushAction(a);
		}
	};

}