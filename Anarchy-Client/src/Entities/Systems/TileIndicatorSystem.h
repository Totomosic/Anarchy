#pragma once
#include "Engine/Scene/Systems/System.h"
#include "../Components/TileIndicator.h"

namespace Anarchy
{

	class TileIndicatorSystem : public Bolt::System<TileIndicatorSystem>
	{
	public:
		void Update(Bolt::EntityManager& entities, Bolt::TimeDelta dt) override;
	};

}