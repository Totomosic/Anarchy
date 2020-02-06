#pragma once
#include "Lib/Entities/EntityCollection.h"

namespace Anarchy
{

	class ClientEntityCollection : public EntityCollection
	{
	private:
		entityid_t m_OwnedEntity;
		EntityHandle m_Camera;

	public:
		ClientEntityCollection(Scene& scene, Layer& layer);

		bool OwnsEntity(entityid_t networkId) const;
		EntityHandle GetCamera() const;

		void SetOwnedEntity(entityid_t networkId);
		void SetCamera(const EntityHandle& camera);

	};

}