#include "clientpch.h"
#include "ClientEntityCollection.h"

namespace Anarchy
{

	ClientEntityCollection::ClientEntityCollection(Scene& scene, Layer& layer) : EntityCollection(scene, layer, true),
		m_OwnedEntity(-1)
	{
	}

	bool ClientEntityCollection::OwnsEntity(entityid_t networkId) const
	{
		return networkId == m_OwnedEntity;
	}

	void ClientEntityCollection::SetOwnedEntity(entityid_t networkId)
	{
		m_OwnedEntity = networkId;
	}

}