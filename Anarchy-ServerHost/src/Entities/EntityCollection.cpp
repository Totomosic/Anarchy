#include "serverpch.h"
#include "EntityCollection.h"

#include "Lib/Components/NetworkId.h"
#include "Components/EntityOwner.h"

namespace Anarchy
{

	EntityCollection::EntityCollection(Scene& gameScene, Layer& gameLayer)
		: m_GameScene(gameScene), m_GameLayer(gameLayer), m_Prefabs(m_GameLayer), m_NetworkIdManager(0, std::numeric_limits<entityid_t>::max() - 1), m_Listener()
	{
		m_Listener = m_GameLayer.Entities().OnEntityDestroyed().AddScopedEventListener([this](Event<EntityDestroyed>& e)
			{
				if (e.Data.Entity.HasComponent<NetworkId>())
				{
					m_NetworkIdManager.ReleaseId(e.Data.Entity.GetComponent<NetworkId>()->Id);
				}
			});
	}

	const Scene& EntityCollection::GetScene() const
	{
		return m_GameScene;
	}

	Scene& EntityCollection::GetScene()
	{
		return m_GameScene;
	}

	const Layer& EntityCollection::GetGameLayer() const
	{
		return m_GameLayer;
	}

	Layer& EntityCollection::GetGameLayer()
	{
		return m_GameLayer;
	}

	EntityHandle EntityCollection::CreateEntity(const EntityHandle& entity)
	{
		entity.Assign<NetworkId>(NetworkId{ m_NetworkIdManager.GetNextId() });
		return entity;
	}

	EntityHandle EntityCollection::CreateEntity(const EntityHandle& entity, connid_t ownerConnectionId)
	{
		EntityHandle e = CreateEntity(entity);
		e.Assign<EntityOwner>(EntityOwner{ ownerConnectionId });
		return e;
	}

	EntityHandle EntityCollection::CreateEntity(prefab_t prefabId)
	{
		return CreateEntity(m_Prefabs.CreateEntity(prefabId, false));
	}

	EntityHandle EntityCollection::CreateEntity(prefab_t prefabId, connid_t ownerConnectionId)
	{
		return CreateEntity(m_Prefabs.CreateEntity(prefabId, false), ownerConnectionId);
	}

	void EntityCollection::RemoveAllOwnedBy(connid_t connectionId)
	{
		for (EntityHandle entity : m_GameLayer.Entities().GetEntitiesWith<EntityOwner>())
		{
			if (entity.GetComponent<EntityOwner>()->ConnectionId == connectionId)
			{
				entity.Destroy();
			}
		}
	}

}