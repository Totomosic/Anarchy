#include "EntityCollection.h"
#include "Components/NetworkId.h"
#include "Components/PrefabId.h"
#include "Components/DimensionId.h"
#include "Components/TilePosition.h"

namespace Anarchy
{

	EntityCollection::EntityCollection(Scene& gameScene, Layer& gameLayer, bool enableGraphics)
		: m_GameScene(gameScene), m_GameLayer(gameLayer), m_Prefabs(m_GameLayer), m_AddGraphicsComponents(enableGraphics), m_OnDestroyListener(), m_EntityMap()
	{
		m_OnDestroyListener = m_GameLayer.Entities().OnEntityDestroyed().AddScopedEventListener([this](Event<EntityDestroyed>& e)
			{
				if (e.Data.Entity.HasComponent<CNetworkId>())
				{
					m_EntityMap.erase(e.Data.Entity.GetComponent<CNetworkId>()->Id);
				}
			}, ListenerPriority::Low);
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

	std::vector<EntityHandle> EntityCollection::GetAllEntities() const
	{
		std::vector<EntityHandle> result;
		for (const auto& pair : m_EntityMap)
		{
			result.push_back(pair.second);
		}
		return result;
	}

	bool EntityCollection::HasEntity(entityid_t networkId) const
	{
		return m_EntityMap.find(networkId) != m_EntityMap.end();
	}

	EntityHandle EntityCollection::GetEntityByNetworkId(entityid_t networkId) const
	{
		BLT_ASSERT(HasEntity(networkId), "Entity with network id does not exist");
		return m_EntityMap.at(networkId);
	}

	EntityHandle EntityCollection::CreateEntity(entityid_t networkId, const EntityHandle& entity)
	{
		entity.Assign<CNetworkId>(CNetworkId{ networkId });
		m_EntityMap[networkId] = entity;
		return entity;
	}

	EntityHandle EntityCollection::CreateEntity(entityid_t networkId, prefab_t prefabId)
	{
		EntityHandle entity = CreateEntity(networkId, m_Prefabs.CreateEntity(prefabId, m_AddGraphicsComponents));
		entity.Assign<CPrefabId>(CPrefabId{ prefabId });
		return entity;
	}

	EntityHandle EntityCollection::CreateFromEntityData(const EntityData& data)
	{
		EntityHandle entity = CreateEntity(data.NetworkId, data.PrefabId);
		entity.Assign<CDimensionId>(CDimensionId{ data.DimensionId });
		if (entity.HasComponent<CTilePosition>())
		{
			entity.GetComponent<CTilePosition>()->Position = data.TilePosition;
		}
		if (entity.HasComponent<Transform>())
		{
			entity.GetTransform()->SetLocalPosition(data.TilePosition.x, data.TilePosition.y, 0.0f);
		}
		return entity;
	}

	void EntityCollection::RemoveEntity(entityid_t networkId)
	{
		if (m_EntityMap.find(networkId) != m_EntityMap.end())
		{
			m_EntityMap[networkId].Destroy();
			m_EntityMap.erase(networkId);
		}
	}

	Vector2i EntityCollection::GetEntityTilePosition(const EntityHandle& entity) const
	{
		Vector2i xy = entity.GetComponent<CTilePosition>()->Position;
		return xy;
	}

	EntityData EntityCollection::GetDataFromEntity(const EntityHandle& entity) const
	{
		EntityData data;
		data.NetworkId = entity.GetComponent<CNetworkId>()->Id;
		data.PrefabId = entity.GetComponent<CPrefabId>()->Id;
		data.DimensionId = entity.GetComponent<CDimensionId>()->Id;
		data.HeightLevel = 0;
		data.Level = 1;
		data.TilePosition = GetEntityTilePosition(entity);
		return data;
	}

}