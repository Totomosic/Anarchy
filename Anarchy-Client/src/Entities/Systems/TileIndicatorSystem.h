#pragma once
#include "Engine/Scene/Systems/System.h"
#include "../Components/TileIndicator.h"

namespace Anarchy
{

	class TileIndicatorSystem : public Bolt::System<TileIndicatorSystem>
	{
	private:
		bool m_ShouldUpdate;
		ScopedEventListener m_Listener;

	public:
		TileIndicatorSystem();

		void ForceUpdate();
		void Update(Bolt::EntityManager& entities, Bolt::TimeDelta dt) override;
	};

}