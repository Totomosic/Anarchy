#pragma once
#include "DebugCommandRegistry.h"
#include "Core/Events/EventEmitter.h"
#include "Core/Events/EventBus.h"

namespace Anarchy
{

	class DebugCommandManager
	{
	public:
		static constexpr char COMMAND_CHAR = '/';

	private:
		DebugCommandRegistry m_Registry;
		EventEmitter<RunDebugCommand> m_Emitter;
		ScopedEventListener m_EventListener;

	public:
		DebugCommandManager();

		void StartStdinListener();

		bool Execute(const std::string& command) const;
		bool Execute(const std::string& commandName, const std::vector<std::string>& args) const;
		bool Execute(const RunDebugCommand& command) const;

	private:
		void EventHandler(Event<RunDebugCommand>& e);
		std::optional<RunDebugCommand> SplitString(const std::string& commandString) const;
		void LogMessage(const std::string& message) const;

		void RegisterCommands();
	};

}