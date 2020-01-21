#pragma once
#include "EntityCommands.h"

namespace Anarchy
{

	class CommandBuffer
	{
	public:
		using CommandHandler = std::function<void(const GenericCommand&)>;

	private:
		std::vector<GenericCommand> m_Commands;
		std::unordered_map<CommandType, CommandHandler> m_Handlers;

	public:
		CommandBuffer();

		const std::vector<GenericCommand>& GetCommands() const;
		void ProcessAllCommands();
		void PushCommand(const GenericCommand& command);
		void Clear();

		template<typename T>
		void PushCommand(const InputCommand<T>& command)
		{
			GenericCommand cmd;
			cmd.CommandType = T::Type;
			cmd.CommandData = std::make_shared<OutputMemoryStream>(sizeof(InputCommand<T>));
			Serialize(*cmd.CommandData, command);
			PushCommand(cmd);
		}

		template<typename T>
		void RegisterHandler(CommandType type, const std::function<void(const InputCommand<T>&)>& callback)
		{
			m_Handlers[type] = [callback](const GenericCommand& command)
			{
				InputCommand<T> cmd;
				InputMemoryStream stream(command.CommandData->GetRemainingDataSize());
				memcpy(stream.GetBufferPtr(), command.CommandData->GetBufferPtr(), stream.GetRemainingDataSize());
				Deserialize(stream, cmd);
				callback(cmd);
			};
		}
	};

}