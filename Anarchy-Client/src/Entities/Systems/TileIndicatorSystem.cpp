#include "clientpch.h"
#include "TileIndicatorSystem.h"
#include "ClientState.h"
#include "Lib/Entities/Components/TilePosition.h"

namespace Anarchy
{
	TileIndicatorSystem::TileIndicatorSystem()
		: m_ShouldUpdate(true), m_Listener()
	{
		m_Listener = Input::Get().OnMouseMoved.AddScopedEventListener([this](Event<MouseMovedEvent>& e)
			{
				m_ShouldUpdate = true;
			});
	}

	void TileIndicatorSystem::ForceUpdate()
	{
		m_ShouldUpdate = true;
	}

	void TileIndicatorSystem::Update(Bolt::EntityManager& entities, Bolt::TimeDelta dt)
	{
		if (m_ShouldUpdate)
		{
			if (ClientState::Get().HasEntities())
			{
				EntityHandle camera = ClientState::Get().GetEntities().GetCamera();
				if (camera)
				{
					ComponentHandle c = camera.GetComponent<Bolt::Camera>();
					Vector4f ndc = { Input::Get().NormalizedMousePosition(), 1.0f };
					Vector4f modelPosition = c->GetProjectionMatrix().Inverse() * ndc;
					Vector4f worldPosition = camera.GetTransform()->TransformMatrix() * modelPosition;
					Vector2i tilePosition = { (int)(worldPosition.x + 0.5f), (int)(worldPosition.y + 0.5f) };
					for (EntityHandle entity : entities.GetEntitiesWith<CTileIndicator, Bolt::Transform>())
					{
						entity.GetComponent<Bolt::Transform>()->SetLocalXY(tilePosition);
						if (entity.HasComponent<CTilePosition>())
						{
							entity.GetComponent<CTilePosition>()->Position = tilePosition;
						}
					}
				}
			}
			m_ShouldUpdate = false;
		}
	}

}
