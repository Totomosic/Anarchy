#pragma once
#include "Lib/Entities/EntityCollection.h"

namespace Anarchy
{

	class ClientEntityCollection : public EntityCollection
	{
	public:
		inline static Vector2i InvalidTile = { -1, -1 };

	private:
		entityid_t m_ControlledEntity;
		EntityHandle m_Camera;
		EntityHandle m_TileIndicator;

	public:
		ClientEntityCollection(Scene& scene, Layer& layer);

		bool IsControllingEntity(entityid_t networkId) const;
		EntityHandle GetCamera() const;

		void SetControlledEntity(entityid_t networkId);
		void SetCamera(const EntityHandle& camera);
		void SetTileIndicator(const EntityHandle& tileIndicator);

		Vector2i GetSelectedTile() const;

		EntityHandle CreateFromEntityState(const EntityState& state) override;
		EntityHandle ApplyEntityState(const EntityState& state) override;

	};

}