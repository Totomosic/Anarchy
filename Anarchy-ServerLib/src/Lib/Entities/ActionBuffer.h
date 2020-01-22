#pragma once
#include "EntityActions.h"

namespace Anarchy
{

	class ActionBuffer
	{
	public:
		using ActionHandler = std::function<void(const GenericAction&)>;

	private:
		std::vector<GenericAction> m_Actions;
		std::unordered_map<ActionType, ActionHandler> m_Handlers;

	public:
		ActionBuffer();

		const std::vector<GenericAction>& GetActions() const;
		void ProcessAllActions();
		void PushAction(const GenericAction& action);
		void Clear();

		template<typename T>
		void PushAction(const InputAction<T>& action)
		{
			GenericAction act;
			act.Action = T::Type;
			act.ActionData = std::make_shared<OutputMemoryStream>(sizeof(InputAction<T>));
			Serialize(*act.ActionData, action);
			PushAction(act);
		}

		template<typename T>
		void RegisterHandler(ActionType type, const std::function<void(const InputAction<T>&)>& callback)
		{
			m_Handlers[type] = [callback](const GenericAction& action)
			{
				InputAction<T> act;
				InputMemoryStream stream(action.ActionData->GetRemainingDataSize());
				memcpy(stream.GetBufferPtr(), action.ActionData->GetBufferPtr(), stream.GetRemainingDataSize());
				Deserialize(stream, act);
				callback(act);
			};
		}
	};

}