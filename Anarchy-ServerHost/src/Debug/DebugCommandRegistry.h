#pragma once
#include "RunDebugCommand.h"

namespace Anarchy
{

	class DebugCommandRegistry
	{
	public:
		using CommandCallback = std::function<void(const RunDebugCommand&)>;

	private:
		std::unordered_map<std::string, CommandCallback> m_Functions;

	public:
		DebugCommandRegistry();

		void Register(const std::string& commandName, const CommandCallback& executor);
		bool Execute(const RunDebugCommand& command) const;
	};

}