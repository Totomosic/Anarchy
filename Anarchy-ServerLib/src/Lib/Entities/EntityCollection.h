#pragma once
#include "Engine/Scene/Scene.h"
#include "ServerLib.h"
#include "Lib/PrefabRegistry.h"
#include "Lib/Entities/EntityState.h"

namespace Anarchy
{
	
	class EntityCollection
	{
	protected:
		Scene& m_GameScene;
		Layer& m_GameLayer;
		PrefabRegistry m_Prefabs;
		bool m_AddGraphicsComponents;
		ScopedEventListener m_OnDestroyListener;

		std::unordered_map<entityid_t, EntityHandle> m_EntityMap;

	public:
		EntityCollection(Scene& gameScene, Layer& gameLayer, bool enableGraphics);
		virtual ~EntityCollection() {}

		const Scene& GetScene() const;
		Scene& GetScene();
		const Layer& GetGameLayer() const;
		Layer& GetGameLayer();

		std::vector<EntityHandle> GetAllEntities() const;
		bool HasEntity(entityid_t networkId) const;
		EntityHandle GetEntityByNetworkId(entityid_t networkId) const;

		virtual EntityHandle CreateEntity(entityid_t networkId, const EntityHandle& entity);
		EntityHandle CreateEntity(entityid_t networkId, prefab_t prefabId);
		virtual EntityHandle CreateFromEntityState(const EntityState& state);
		virtual EntityHandle ApplyEntityState(const EntityState& state);

		void RemoveEntity(entityid_t networkId);

		Vector2i GetEntityTilePosition(const EntityHandle& entity) const;
		EntityState GetStateFromEntity(const EntityHandle& entity) const;
	};

}