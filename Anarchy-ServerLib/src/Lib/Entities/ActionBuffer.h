#pragma once
#include "EntityActions.h"

namespace Anarchy
{

	class ActionBuffer
	{
	public:
		using ActionHandler = std::function<void(const GenericAction&, bool)>;

	private:
		std::vector<GenericAction> m_Actions;
		// Actions that should be sent over network
		std::vector<GenericAction> m_NetworkActions;
		std::unordered_map<ActionType, ActionHandler> m_Handlers;

	public:
		ActionBuffer();

		const std::vector<GenericAction>& GetAllActions() const;
		const std::vector<GenericAction>& GetNetworkActions() const;
		void ProcessAllActions();
		void PushAction(const GenericAction& action, bool sendOverNetwork = true);
		void Clear();

		template<typename T>
		void PushAction(const InputAction<T>& action, bool sendOverNetwork = true)
		{
			GenericAction act;
			act.Action = T::Type;
			act.ActionData = std::make_shared<OutputMemoryStream>(sizeof(InputAction<T>));
			Serialize(*act.ActionData, action);
			PushAction(act);
		}

		template<typename T>
		void RegisterHandler(ActionType type, const std::function<void(const InputAction<T>&, bool)>& callback)
		{
			m_Handlers[type] = [callback](const GenericAction& action, bool fromNetwork)
			{
				InputAction<T> act;
				InputMemoryStream stream(action.ActionData->GetRemainingDataSize());
				memcpy(stream.GetBufferPtr(), action.ActionData->GetBufferPtr(), stream.GetRemainingDataSize());
				Deserialize(stream, act);
				callback(act, fromNetwork);
			};
		}
	};

}