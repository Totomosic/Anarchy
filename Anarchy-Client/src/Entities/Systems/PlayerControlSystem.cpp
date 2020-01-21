#include "clientpch.h"
#include "PlayerControlSystem.h"

#include "Lib/Entities/Components/NetworkId.h"
#include "Lib/Entities/Components/TilePosition.h"

namespace Anarchy
{

	PlayerControlSystem::PlayerControlSystem(CommandBuffer* commandBuffer)
		: m_CommandBuffer(commandBuffer)
	{
	}

	void PlayerControlSystem::Update(EntityManager& manager, TimeDelta dt)
	{
		if (m_CommandBuffer != nullptr)
		{
			for (EntityHandle entity : manager.GetEntitiesWith<PlayerController, TilePosition, NetworkId>())
			{
				Vector2i direction = { 0, 0 };
				if (Input::Get().KeyDown(Keycode::W))
				{
					direction = { 0, 1 };
				}
				else if (Input::Get().KeyDown(Keycode::S))
				{
					direction = { 0, -1 };
				}
				else if (Input::Get().KeyDown(Keycode::D))
				{
					direction = { 1, 0 };
				}
				else if (Input::Get().KeyDown(Keycode::A))
				{
					direction = { -1, 0 };
				}
				
				if (direction.x != 0 || direction.y != 0)
				{
					entityid_t networkId = entity.GetComponent<NetworkId>()->Id;
					InputCommand<TileMovement> command;
					command.NetworkId = networkId;
					command.Command.Movement = direction;
					command.Command.Speed = entity.GetComponent<PlayerController>()->Speed;
					m_CommandBuffer->PushCommand(command);
				}
			}
		}
	}

}