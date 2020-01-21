#include "CommandBuffer.h"

namespace Anarchy
{

	CommandBuffer::CommandBuffer()
		: m_Commands(), m_Handlers()
	{
	}

	const std::vector<GenericCommand>& CommandBuffer::GetCommands() const
	{
		return m_Commands;
	}

	void CommandBuffer::ProcessAllCommands()
	{
		for (const GenericCommand& command : m_Commands)
		{
			if (m_Handlers.find(command.CommandType) != m_Handlers.end())
			{
				m_Handlers.at(command.CommandType)(command);
			}
		}
		m_Commands.clear();
	}

	void CommandBuffer::PushCommand(const GenericCommand& command)
	{
		m_Commands.push_back(command);
	}

	void CommandBuffer::Clear()
	{
		m_Commands.clear();
	}

}