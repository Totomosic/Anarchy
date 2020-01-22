#include "ActionBuffer.h"

namespace Anarchy
{

	ActionBuffer::ActionBuffer()
		: m_Actions(), m_Handlers()
	{
	}

	const std::vector<GenericAction>& ActionBuffer::GetActions() const
	{
		return m_Actions;
	}

	void ActionBuffer::ProcessAllActions()
	{
		for (const GenericAction& command : m_Actions)
		{
			if (m_Handlers.find(command.Action) != m_Handlers.end())
			{
				m_Handlers.at(command.Action)(command);
			}
		}
		m_Actions.clear();
	}

	void ActionBuffer::PushAction(const GenericAction& action)
	{
		m_Actions.push_back(action);
	}

	void ActionBuffer::Clear()
	{
		m_Actions.clear();
	}

}