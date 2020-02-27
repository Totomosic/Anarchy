#include "serverpch.h"
#include "ServerEntityCollection.h"

#include "Components/EntityOwner.h"
#include "Lib/Entities/Components/NetworkId.h"

namespace Anarchy
{

	ServerEntityCollection::ServerEntityCollection(Scene& scene, Layer& layer) : EntityCollection(scene, layer, false),
		m_NetworkIdManager(0, std::numeric_limits<entityid_t>::max() - 1), m_Listener(), m_DirtyEntities()
	{
		m_Listener = m_GameLayer.Entities().OnEntityDestroyed().AddScopedEventListener([this](Event<EntityDestroyed>& e)
			{
				if (e.Data.Entity.HasComponent<CNetworkId>())
				{
					entityid_t networkId = e.Data.Entity.GetComponent<CNetworkId>()->Id;
					m_NetworkIdManager.ReleaseId(networkId);
				}
			});
	}

	entityid_t ServerEntityCollection::GetNextEntityId() const
	{
		return m_NetworkIdManager.GetNextId();
	}

	EntityHandle ServerEntityCollection::CreateEntity(entityid_t networkId, const EntityHandle& entity)
	{
		return EntityCollection::CreateEntity(networkId, entity);
	}

	EntityHandle ServerEntityCollection::CreateEntity(entityid_t networkId, const EntityHandle& entity, connid_t ownerConnectionId)
	{
		EntityHandle e = CreateEntity(networkId, entity);
		e.Assign<CEntityOwner>(CEntityOwner{ ownerConnectionId });
		return e;
	}

	EntityHandle ServerEntityCollection::CreateEntity(entityid_t networkId, prefab_t prefabId, connid_t ownerConnectionId)
	{
		return CreateEntity(networkId, m_Prefabs.CreateEntity(prefabId, false), ownerConnectionId);
	}

	EntityHandle ServerEntityCollection::CreateFromEntityData(const EntityState& state, connid_t ownerConnectionId)
	{
		EntityHandle entity = EntityCollection::CreateFromEntityState(state);
		if (ownerConnectionId != InvalidConnectionId)
		{
			entity.Assign<CEntityOwner>(CEntityOwner{ ownerConnectionId });
		}
		return entity;
	}

	std::vector<entityid_t> ServerEntityCollection::GetAllIdsOwnedBy(connid_t connectionId) const
	{
		std::vector<entityid_t> result;
		for (EntityHandle entity : m_GameLayer.Entities().GetEntitiesWith<CEntityOwner, CNetworkId>())
		{
			if (entity.GetComponent<CEntityOwner>()->ConnectionId == connectionId)
			{
				result.push_back(entity.GetComponent<CNetworkId>()->Id);
			}
		}
		return result;
	}

	const std::vector<entityid_t>& ServerEntityCollection::GetDirtyEntities() const
	{
		return m_DirtyEntities;
	}

	void ServerEntityCollection::SetEntityDirty(entityid_t entityId)
	{
		if (std::find(m_DirtyEntities.begin(), m_DirtyEntities.end(), entityId) == m_DirtyEntities.end())
		{
			m_DirtyEntities.push_back(entityId);
		}
	}

	void ServerEntityCollection::ClearDirtyEntities()
	{
		m_DirtyEntities.clear();
	}

}