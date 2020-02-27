#include "EntityCollection.h"
#include "Components/NetworkId.h"
#include "Components/PrefabId.h"
#include "Components/DimensionId.h"
#include "Components/TilePosition.h"
#include "Components/EntityName.h"
#include "Components/Health.h"

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
		if (HasEntity(networkId))
		{
			return m_EntityMap.at(networkId);
		}
		return EntityHandle();
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

	EntityHandle EntityCollection::CreateFromEntityState(const EntityState& state)
	{
		EntityHandle entity = CreateEntity(state.NetworkId, state.PrefabId);
		entity.Assign<CDimensionId>(CDimensionId{ state.DimensionId });
		if (entity.HasComponent<CTilePosition>())
		{
			entity.GetComponent<CTilePosition>()->Position = state.TilePosition;
		}
		if (entity.HasComponent<Transform>())
		{
			entity.GetTransform()->SetLocalPosition(state.TilePosition.x, state.TilePosition.y, 0.0f);
		}
		if (!state.Name.empty())
		{
			entity.Assign<CEntityName>(CEntityName{ state.Name });
		}
		CLifeForce lf;
		lf.CurrentHealth = state.CurrentHealth;
		lf.MaxHealth = state.MaxHealth;
		lf.Shield = state.CurrentShield;
		entity.Assign<CLifeForce>(lf);
		return entity;
	}

	EntityHandle EntityCollection::ApplyEntityState(const EntityState& state)
	{
		EntityHandle entity = GetEntityByNetworkId(state.NetworkId);
		if (entity)
		{
			entity.GetComponent<CPrefabId>()->Id = state.PrefabId;
			entity.GetComponent<CDimensionId>()->Id = state.DimensionId;
			entity.GetComponent<CTilePosition>()->Position = state.TilePosition;
			ComponentHandle lf = entity.GetComponent<CLifeForce>();
			lf->CurrentHealth = state.CurrentHealth;
			lf->MaxHealth = state.MaxHealth;
			lf->Shield = state.CurrentShield;
			if (state.Name.empty())
			{
				if (entity.HasComponent<CEntityName>())
					entity.Remove<CEntityName>();
			}
			else
			{
				if (!entity.HasComponent<CEntityName>())
					entity.Assign<CEntityName>(CEntityName{ state.Name });
				else
					entity.GetComponent<CEntityName>()->Name = state.Name;
			}
		}
		return entity;
	}

	void EntityCollection::RemoveEntity(entityid_t networkId)
	{
		if (m_EntityMap.find(networkId) != m_EntityMap.end())
		{
			m_EntityMap[networkId].Destroy();
		}
	}

	Vector2i EntityCollection::GetEntityTilePosition(const EntityHandle& entity) const
	{
		Vector2i xy = entity.GetComponent<CTilePosition>()->Position;
		return xy;
	}

	EntityState EntityCollection::GetStateFromEntity(const EntityHandle& entity) const
	{
		EntityState state;
		state.NetworkId = entity.GetComponent<CNetworkId>()->Id;
		state.PrefabId = entity.GetComponent<CPrefabId>()->Id;
		state.DimensionId = entity.GetComponent<CDimensionId>()->Id;
		state.HeightLevel = 0;
		state.Level = 1;
		state.TilePosition = GetEntityTilePosition(entity);
		state.TileSize = { 1, 1 };
		if (entity.HasComponent<CEntityName>())
		{
			state.Name = entity.GetComponent<CEntityName>()->Name;
		}
		ComponentHandle lifeforce = entity.GetComponent<CLifeForce>();
		state.MaxHealth = lifeforce->MaxHealth;
		state.CurrentHealth = lifeforce->CurrentHealth;
		state.CurrentShield = lifeforce->Shield;
		return state;
	}

}