#pragma once
#include "Engine/Scene/Systems/System.h"
#include "../Components/PlayerController.h"
#include "../ActionHistory.h"
#include "../ActionRegistry.h"

namespace Anarchy
{

	class PlayerControlSystem : public System<PlayerControlSystem>
	{
	private:
		ActionHistory* m_ActionHistory;
		ActionRegistry* m_ActionRegistry;

	public:
		PlayerControlSystem(ActionHistory* actionHistory, ActionRegistry* actionRegistry);

		void Update(EntityManager& manager, TimeDelta dt) override;
	};

}