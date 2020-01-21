#pragma once
#include "Engine/Scene/Systems/System.h"
#include "../Components/PlayerController.h"
#include "Lib/Entities/CommandBuffer.h"

namespace Anarchy
{

	class PlayerControlSystem : public System<PlayerControlSystem>
	{
	private:
		CommandBuffer* m_CommandBuffer;

	public:
		PlayerControlSystem(CommandBuffer* commandBuffer);

		void Update(EntityManager& manager, TimeDelta dt) override;
	};

}