#pragma once
#include "DebugCommandRegistry.h"
#include "Core/Events/EventEmitter.h"
#include "Core/Events/EventBus.h"
#include "Engine/Scene/EntityManager.h"

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

		void LogEntity(const EntityHandle& entity) const;
		
		template<typename T>
		typename std::enable_if_t<!std::is_unsigned_v<T>, bool> GetIntegerArg(const std::string& arg, T* value) const
		{
			try
			{
				int64_t v = std::stoll(arg);
				*value = (T)v;
				return true;
			}
			catch (std::invalid_argument e)
			{
			}
			catch (std::out_of_range e)
			{
			}
			return false;
		}

		template<typename T>
		typename std::enable_if_t<std::is_unsigned_v<T>, bool> GetIntegerArg(const std::string& arg, T* value) const
		{
			try
			{
				uint64_t v = std::stoull(arg);
				*value = (T)v;
				return true;
			}
			catch (std::invalid_argument e)
			{
			}
			catch (std::out_of_range e)
			{
			}
			return false;
		}
	};

}