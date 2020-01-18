#include "serverpch.h"
#include "DebugCommandManager.h"
#include "Events.h"
#include "Core/Tasks/TaskManager.h"

#include "ServerState.h"

namespace Anarchy
{

	Anarchy::DebugCommandManager::DebugCommandManager()
		: m_Registry(), m_Emitter(EventManager::Get().Bus().GetEmitter<RunDebugCommand>(ServerEvents::RunDebugCommand)), m_EventListener()
	{
		m_EventListener = m_Emitter.AddScopedEventListener(std::bind(&DebugCommandManager::EventHandler, this, std::placeholders::_1));
		RegisterCommands();
	}

	void DebugCommandManager::StartStdinListener()
	{
		Task t = TaskManager::Get().Run([this]()
			{
				char buffer[4096];
				while (true)
				{					
					std::cin.getline(buffer, sizeof(buffer));
					std::optional<RunDebugCommand> command = SplitString(buffer);
					if (command)
					{
						RunDebugCommand c = command.value();
						m_Emitter.Emit(c);
					}
				}
			});
	}

	bool DebugCommandManager::Execute(const std::string& command) const
	{
		std::optional<RunDebugCommand> cmd = SplitString(command);
		if (cmd)
		{
			return Execute(cmd.value());
		}
		return false;
	}

	bool DebugCommandManager::Execute(const std::string& commandName, const std::vector<std::string>& args) const
	{
		return Execute({ commandName, args });
	}

	bool DebugCommandManager::Execute(const RunDebugCommand& command) const
	{
		return m_Registry.Execute(command);
	}

	void DebugCommandManager::EventHandler(Event<RunDebugCommand>& e)
	{
		Execute(e.Data);
	}

	std::optional<RunDebugCommand> DebugCommandManager::SplitString(const std::string& commandString) const
	{
		if (commandString.empty() || commandString.front() != COMMAND_CHAR)
		{
			return {};
		}
		size_t space = commandString.find(' ');
		std::string cmd = commandString.substr(1);
		if (space == std::string::npos)
		{
			return RunDebugCommand{ cmd, {} };
		}
		RunDebugCommand result;
		result.Command = cmd.substr(0, space - 1);
		result.Args = blt::split(cmd.substr(space), ' ');
		return result;
	}

	void DebugCommandManager::LogMessage(const std::string& message) const
	{
		std::cout << message << std::endl;
	}

	void DebugCommandManager::RegisterCommands()
	{
		m_Registry.Register("help", [this](const RunDebugCommand& command)
			{
				LogMessage("Commands:");
				LogMessage("/status [ConnectionId]");
				LogMessage("/list");
				LogMessage("/disconnect ConnectionId");
			});

		m_Registry.Register("status", [this](const RunDebugCommand& command)
			{
				if (command.Args.size() == 0)
				{
					const SocketAddress& address = ServerState::Get().GetSocket().GetAddress();
					std::vector<ClientConnection*> connections = ServerState::Get().GetConnections().GetConnections();
					LogMessage("Server running on " + address.ToString());
					LogMessage("Serving " + std::to_string(connections.size()) + " connections");
				}
				else if (command.Args.size() == 1)
				{
					try
					{
						connid_t connectionId = std::stoi(command.Args[0]);
						if (ServerState::Get().GetConnections().HasConnection(connectionId))
						{
							ClientConnection& connection = ServerState::Get().GetConnections().GetConnection(connectionId);
							LogMessage("Active Connection: ConnectionId=" + std::to_string(connection.GetConnectionId()));
							LogMessage("Address: " + connection.GetAddress().ToString());
							LogMessage("Average RTT: " + std::to_string(connection.GetAverageRTT()) + "ms");
							return;
						}
						BLT_ERROR("No connection with id {} exists", connectionId);
					}
					catch (std::invalid_argument e)
					{
						BLT_ERROR("Invalid ConnectionId argument");
					}
				}
			});

		m_Registry.Register("list", [this](const RunDebugCommand& command)
			{
				std::vector<ClientConnection*> connections = ServerState::Get().GetConnections().GetConnections();
				if (connections.size() > 0)
				{
					LogMessage("Active Connections:");
					for (ClientConnection* connection : connections)
					{
						LogMessage("Address: " + connection->GetAddress().ToString() + " ConnectionId: " + std::to_string(connection->GetConnectionId()));
					}
				}
				else
				{
					LogMessage("No active connections.");
				}
			});

		m_Registry.Register("disconnect", [this](const RunDebugCommand& command)
			{
				if (command.Args.size() != 1)
				{
					BLT_ERROR("Invalid usage of /disconnect connectionId");
					return;
				}
				try
				{
					connid_t connectionId = std::stoi(command.Args[0]);
					if (ServerState::Get().GetConnections().HasConnection(connectionId))
					{
						ServerState::Get().GetSocketApi().ForceDisconnectConnections({ connectionId });
						return;
					}
					BLT_ERROR("No connection with id {} exists", connectionId);
				}
				catch (std::invalid_argument e)
				{
					BLT_ERROR("Invalid ConnectionId argument");
				}				
			});
	}

}