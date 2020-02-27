#include "ActionQueue.h"

namespace Anarchy
{

	ActionQueue::ActionQueue()
		: m_Actions()
	{
	}

	const std::vector<GenericAction>& ActionQueue::GetAllActions() const
	{
		return m_Actions;
	}

	void ActionQueue::PushAction(const GenericAction& action)
	{
		m_Actions.push_back(action);
	}

	void ActionQueue::Clear()
	{
		m_Actions.clear();
	}

}
