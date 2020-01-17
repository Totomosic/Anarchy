#include "serverpch.h"
#include "DebugCommandRegistry.h"

namespace Anarchy
{

	DebugCommandRegistry::DebugCommandRegistry()
		: m_Functions()
	{
	}

	void DebugCommandRegistry::Register(const std::string& commandName, const CommandCallback& executor)
	{
		m_Functions[commandName] = executor;
	}

	bool DebugCommandRegistry::Execute(const RunDebugCommand& command) const
	{
		if (m_Functions.find(command.Command) != m_Functions.end())
		{
			m_Functions.at(command.Command)(command);
			return true;
		}
		BLT_ERROR("Invalid command {}", command.Command);
		return false;
	}

}