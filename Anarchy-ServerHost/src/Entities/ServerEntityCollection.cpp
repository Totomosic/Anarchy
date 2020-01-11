#include "serverpch.h"
#include "ServerEntityCollection.h"

#include "Components/EntityOwner.h"
#include "Lib/Entities/Components/NetworkId.h"

namespace Anarchy
{

	ServerEntityCollection::ServerEntityCollection(Scene& scene, Layer& layer) : EntityCollection(scene, layer, false),
		m_NetworkIdManager(0, std::numeric_limits<entityid_t>::max() - 1), m_Listener()
	{
		m_Listener = m_GameLayer.Entities().OnEntityDestroyed().AddScopedEventListener([this](Event<EntityDestroyed>& e)
			{
				if (e.Data.Entity.HasComponent<NetworkId>())
				{
					m_NetworkIdManager.ReleaseId(e.Data.Entity.GetComponent<NetworkId>()->Id);
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
		e.Assign<EntityOwner>(EntityOwner{ ownerConnectionId });
		return e;
	}

	EntityHandle ServerEntityCollection::CreateEntity(entityid_t networkId, prefab_t prefabId, connid_t ownerConnectionId)
	{
		return CreateEntity(networkId, m_Prefabs.CreateEntity(prefabId, false), ownerConnectionId);
	}

	EntityHandle ServerEntityCollection::CreateFromEntityData(const EntityData& data, connid_t ownerConnectionId)
	{
		EntityHandle entity = EntityCollection::CreateFromEntityData(data);
		if (ownerConnectionId != InvalidConnectionId)
		{
			entity.Assign<EntityOwner>(EntityOwner{ ownerConnectionId });
		}
		return entity;
	}

	std::vector<entityid_t> ServerEntityCollection::GetAllIdsOwnedBy(connid_t connectionId) const
	{
		std::vector<entityid_t> result;
		for (EntityHandle entity : m_GameLayer.Entities().GetEntitiesWith<EntityOwner, NetworkId>())
		{
			if (entity.GetComponent<EntityOwner>()->ConnectionId == connectionId)
			{
				result.push_back(entity.GetComponent<NetworkId>()->Id);
			}
		}
		return result;
	}

}