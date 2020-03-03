#include "clientpch.h"
#include "ActionHistory.h"
#include "ClientState.h"

namespace Anarchy
{

	ActionHistory::ActionHistory()
		: m_Actions(), m_NextActionId(0)
	{
	}

	seqid_t ActionHistory::GetNextActionId()
	{
		return m_NextActionId++;
	}

	seqid_t ActionHistory::PeekNextActionId() const
	{
		return m_NextActionId;
	}

	const std::vector<GenericAction>& ActionHistory::GetAllActions() const
	{
		return m_Actions;
	}

	std::vector<GenericAction> ActionHistory::GetActionsAfter(seqid_t actionId) const
	{
		if (m_Actions.empty())
		{
			return {};
		}
		for (int i = 0; i < m_Actions.size(); i++)
		{
			const GenericAction& action = m_Actions[i];
			if (IsSeqIdGreater(action.ActionId, actionId))
			{
				return std::vector<GenericAction>(m_Actions.begin() + i, m_Actions.end());
			}
		}
		return {};
	}

	bool ActionHistory::ContainsAction(seqid_t actionId) const
	{
		return std::find_if(m_Actions.begin(), m_Actions.end(), [actionId](const GenericAction& action)
			{
				return action.ActionId == actionId;
			}) != m_Actions.end();
	}

	void ActionHistory::PushAction(const GenericAction& action)
	{
		if (ClientState::Get().HasConnection())
		{
			ClientState::Get().GetConnection().GetSocketApi().SendAction(action);
		}
		if (m_Actions.empty())
		{
			m_Actions.push_back(action);
		}
		else
		{
			for (int i = m_Actions.size() - 1; i >= 0; i--)
			{
				if (IsSeqIdGreater(action.ActionId, m_Actions[i].ActionId))
				{
					m_Actions.insert(m_Actions.begin() + i + 1, action);
					break;
				}
			}
		}
	}

	void ActionHistory::ClearAllActions()
	{
		m_Actions.clear();
	}

	void ActionHistory::ClearActionsBeforeIncluding(seqid_t actionId)
	{
		if (m_Actions.size() > 0)
		{
			for (int i = m_Actions.size() - 1; i >= 0; i--)
			{
				const GenericAction& action = m_Actions[i];
				if (actionId == action.ActionId || IsSeqIdGreater(actionId, action.ActionId))
				{
					m_Actions.erase(m_Actions.begin(), m_Actions.begin() + i + 1);
					break;
				}
			}
		}
	}

	void ActionHistory::Reset()
	{
		ClearAllActions();
		m_NextActionId = 0;
	}

}
