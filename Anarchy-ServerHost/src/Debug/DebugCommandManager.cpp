#include "serverpch.h"
#include "DebugCommandManager.h"
#include "Events.h"
#include "Core/Tasks/TaskManager.h"

#include "ServerState.h"

#include "Lib/Entities/Components/TilePosition.h"
#include "Lib/Entities/Components/NetworkId.h"
#include "Lib/Entities/Components/DimensionId.h"
#include "Lib/Entities/Components/Health.h"
#include "Lib/Entities/Components/EntityName.h"

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
				LogMessage("/throttle [BytesPerSecond]");
				LogMessage("/tps [tps]");
				LogMessage("/entities [ConnectionId]");
				LogMessage("/kill EntityId");
				LogMessage("/tp EntityId x y");
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
					connid_t connectionId;
					if (GetIntegerArg(command.Args[0], &connectionId))
					{
						if (ServerState::Get().GetConnections().HasConnection(connectionId))
						{
							ClientConnection& connection = ServerState::Get().GetConnections().GetConnection(connectionId);
							LogMessage("Active Connection: ConnectionId=" + std::to_string(connection.GetConnectionId()));
							LogMessage("Address: " + connection.GetAddress().ToString());
							LogMessage("Average RTT: " + std::to_string(connection.GetAverageRTT()) + "ms");
						}
						else
						{
							BLT_ERROR("No connection with id {} exists", connectionId);
						}
					}
					else
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
				connid_t connectionId;
				if (GetIntegerArg(command.Args[0], &connectionId))
				{
					if (ServerState::Get().GetConnections().HasConnection(connectionId))
						ServerState::Get().GetSocketApi().ForceDisconnectConnections({ connectionId });
					else
						BLT_ERROR("No connection with id {} exists", connectionId);
				}
				else
					BLT_ERROR("Invalid ConnectionId argument");
			});

		m_Registry.Register("throttle", [this](const RunDebugCommand& command)
			{
				if (command.Args.size() == 1)
				{
					int64_t bytesPerSecond;
					if (GetIntegerArg(command.Args[0], &bytesPerSecond))
					{
						if (bytesPerSecond < 0)
						{
							BLT_ERROR("Invalid BytesPerSecond argument");
						}
						else
						{
							ServerState::Get().GetSocket().SetMaxBytesPerSecond(bytesPerSecond);
							LogMessage("Set server to maximum of " + std::to_string(bytesPerSecond) + " bytes per second");
						}
					}
					else
					{
						BLT_ERROR("Invalid BytesPerSecond argument");
					}
				}
				else if (command.Args.size() == 0)
				{
					ServerState::Get().GetSocket().SetMaxBytesPerSecond(1024 * 1024 * 1);
					LogMessage("Removed server throttling");
				}
				else
				{
					BLT_ERROR("Invalid args for /throttle");
				}
			});

		m_Registry.Register("tps", [this](const RunDebugCommand& command)
			{
				if (command.Args.size() != 1)
				{
					LogMessage("Ticks per second: " + std::to_string((int)round(Time::Get().FramesPerSecond())));
				}
				else
				{
					int tps;
					if (GetIntegerArg(command.Args[0], &tps))
					{
						ServerState::Get().SetTargetTicksPerSecond(tps);
						LogMessage("Set target tps: " + std::to_string(tps));
					}
					else
					{
						BLT_ERROR("Invalid tps argument");
					}
				}
			});

		m_Registry.Register("entities", [this](const RunDebugCommand& command)
			{
				ServerEntityCollection& entities = ServerState::Get().GetEntities();
				if (command.Args.size() == 0)
				{
					// All entities
					std::vector<EntityHandle> allEntities = entities.GetAllEntities();
					LogMessage("Currently " + std::to_string(allEntities.size()) + " entities");
					for (EntityHandle entity : allEntities)
					{
						LogEntity(entity);
					}
				}
				else
				{
					connid_t connectionId;
					if (GetIntegerArg(command.Args[0], &connectionId))
					{
						if (connectionId >= 0 && ServerState::Get().GetConnections().HasConnection(connectionId))
						{
							std::vector<entityid_t> allEntities = entities.GetAllIdsOwnedBy(connectionId);
							LogMessage("Connection with id " + std::to_string(connectionId) + " owns " + std::to_string(allEntities.size()) + " entities");
							for (entityid_t id : allEntities)
							{
								LogEntity(entities.GetEntityByNetworkId(id));
							}
						}
						else
						{
							BLT_ERROR("No connection with id {}", connectionId);
						}
					}
					else
					{
						BLT_ERROR("Invalid ConnectionId argument");
					}
				}
			});

		m_Registry.Register("kill", [this](const RunDebugCommand& command)
			{
				if (command.Args.size() == 1)
				{
					ServerEntityCollection& entities = ServerState::Get().GetEntities();
					entityid_t networkId;
					if (GetIntegerArg(command.Args[0], &networkId))
					{
						if (networkId >= 0 && entities.HasEntity(networkId))
						{
							MEntityDied message;
							message.NetworkId = networkId;
							ServerState::Get().GetSocketApi().EntityDied(ServerState::Get().GetConnections().GetAllConnectionIds(), message);
							entities.RemoveEntity(networkId);
						}
						else
						{
							BLT_ERROR("No entity with id {} exists", networkId);
						}
					}
					else
					{
						BLT_ERROR("Invalid EntityId argument");
					}
				}
				else
				{
					BLT_ERROR("Invalid argument count");
				}
			});

		m_Registry.Register("tp", [this](const RunDebugCommand& command)
			{
				if (command.Args.size() == 3)
				{
					entityid_t networkId;
					if (GetIntegerArg(command.Args[0], &networkId))
					{
						int x;
						int y;
						if (GetIntegerArg(command.Args[1], &x) && GetIntegerArg(command.Args[2], &y))
						{
							EntityHandle entity = ServerState::Get().GetEntities().GetEntityByNetworkId(networkId);
							if (entity)
							{
								entity.GetComponent<CTilePosition>()->Position = { x, y };
							}
						}
					}
				}
			});
	}

	void DebugCommandManager::LogEntity(const EntityHandle& entity) const
	{
		ComponentHandle tilePosition = entity.GetComponent<CTilePosition>();
		ComponentHandle networkId = entity.GetComponent<CNetworkId>();
		ComponentHandle dimensionId = entity.GetComponent<CDimensionId>();
		ComponentHandle lifeforce = entity.GetComponent<CLifeForce>();
		std::string message = "EntityId=" + std::to_string(networkId->Id) + ", DimensionId=" + std::to_string(dimensionId->Id) + ", Position=[" + std::to_string(tilePosition->Position.x) + ", " + std::to_string(tilePosition->Position.y) + "]";
		if (entity.HasComponent<CEntityName>())
		{
			ComponentHandle name = entity.GetComponent<CEntityName>();
			message += ", Name=\"" + name->Name + '"';
		}
		LogMessage(message);
	}

}