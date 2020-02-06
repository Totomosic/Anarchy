#include "clientpch.h"
#include "ClientEntityCollection.h"

namespace Anarchy
{

	ClientEntityCollection::ClientEntityCollection(Scene& scene, Layer& layer) : EntityCollection(scene, layer, true),
		m_OwnedEntity(InvalidNetworkId), m_Camera()
	{
	}

	bool ClientEntityCollection::OwnsEntity(entityid_t networkId) const
	{
		return networkId == m_OwnedEntity;
	}

	EntityHandle ClientEntityCollection::GetCamera() const
	{
		return m_Camera;
	}

	void ClientEntityCollection::SetOwnedEntity(entityid_t networkId)
	{
		m_OwnedEntity = networkId;
		if (m_OwnedEntity != InvalidNetworkId && m_Camera)
		{
			m_Camera.GetTransform()->SetParent(GetEntityByNetworkId(m_OwnedEntity).GetTransform().Get());
		}
		else if (m_Camera)
		{
			m_Camera.GetTransform()->SetParent(nullptr);
		}
	}

	void ClientEntityCollection::SetCamera(const EntityHandle& camera)
	{
		m_Camera = camera;
		if (m_OwnedEntity != InvalidNetworkId)
		{
			m_Camera.GetTransform()->SetParent(GetEntityByNetworkId(m_OwnedEntity).GetTransform().Get());
		}
	}

}