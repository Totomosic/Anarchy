#pragma once
#include "Engine/Scene/Systems/System.h"
#include "../Components/PlayerController.h"
#include "Lib/Entities/ActionBuffer.h"

namespace Anarchy
{

	class PlayerControlSystem : public System<PlayerControlSystem>
	{
	private:
		ActionBuffer* m_ActionBuffer;

	public:
		PlayerControlSystem(ActionBuffer* actionBuffer);

		void Update(EntityManager& manager, TimeDelta dt) override;
	};

}