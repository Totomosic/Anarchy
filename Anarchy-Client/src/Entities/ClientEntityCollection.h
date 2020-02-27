#pragma once
#include "Lib/Entities/EntityCollection.h"

namespace Anarchy
{

	class ClientEntityCollection : public EntityCollection
	{
	private:
		entityid_t m_ControlledEntity;
		EntityHandle m_Camera;

	public:
		ClientEntityCollection(Scene& scene, Layer& layer);

		bool IsControllingEntity(entityid_t networkId) const;
		EntityHandle GetCamera() const;

		void SetControlledEntity(entityid_t networkId);
		void SetCamera(const EntityHandle& camera);

		EntityHandle CreateFromEntityState(const EntityState& state) override;

	};

}