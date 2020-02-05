#pragma once
#include "Lib/Entities/EntityCollection.h"

namespace Anarchy
{

	class ClientEntityCollection : public EntityCollection
	{
	private:
		entityid_t m_OwnedEntity;

	public:
		ClientEntityCollection(Scene& scene, Layer& layer);

		bool OwnsEntity(entityid_t networkId) const;
		void SetOwnedEntity(entityid_t networkId);

	};

}