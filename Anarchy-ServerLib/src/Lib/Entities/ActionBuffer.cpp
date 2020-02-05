#include "ActionBuffer.h"

namespace Anarchy
{

	ActionBuffer::ActionBuffer()
		: m_Actions(), m_NetworkActions(), m_Handlers()
	{
	}

	const std::vector<GenericAction>& ActionBuffer::GetAllActions() const
	{
		return m_Actions;
	}

	const std::vector<GenericAction>& ActionBuffer::GetNetworkActions() const
	{
		return m_NetworkActions;
	}

	void ActionBuffer::ProcessAllActions()
	{
		for (const GenericAction& action : GetAllActions())
		{
			if (m_Handlers.find(action.Action) != m_Handlers.end())
			{
				m_Handlers.at(action.Action)(action, std::find_if(m_NetworkActions.begin(), m_NetworkActions.end(), [&action](const GenericAction& a)
					{
						return a.ActionData.get() == action.ActionData.get();
					}) == m_NetworkActions.end());
			}
		}
	}

	void ActionBuffer::PushAction(const GenericAction& action, bool sendOverNetwork)
	{
		m_Actions.push_back(action);
		if (sendOverNetwork)
		{
			m_NetworkActions.push_back(action);
		}
	}

	void ActionBuffer::Clear()
	{
		m_Actions.clear();
		m_NetworkActions.clear();
	}

}