#pragma once
#include "Lib/Entities/EntityCollection.h"

namespace Anarchy
{

	class ServerEntityCollection : public EntityCollection
	{
	private:
		IdManager<entityid_t> m_NetworkIdManager;
		ScopedEventListener m_Listener;

	public:
		ServerEntityCollection(Scene& scene, Layer& layer);

		entityid_t GetNextEntityId() const;

		virtual EntityHandle CreateEntity(entityid_t networkId, const EntityHandle& entity) override;
		EntityHandle CreateEntity(entityid_t networkId, const EntityHandle& entity, connid_t ownerConnectionId);
		EntityHandle CreateEntity(entityid_t networkId, prefab_t prefabId, connid_t ownerConnectionId);
		EntityHandle CreateFromEntityData(const EntityData& data, connid_t ownerConnectionId);
		void RemoveAllOwnedBy(connid_t connectionId);
	};

}