#pragma once
#include "serverpch.h"
#include "ServerLib.h"
#include "Lib/PrefabRegistry.h"

namespace Anarchy
{
	
	class EntityCollection
	{
	private:
		Scene& m_GameScene;
		Layer& m_GameLayer;
		PrefabRegistry m_Prefabs;
		IdManager<entityid_t> m_NetworkIdManager;

		ScopedEventListener m_Listener;

	public:
		EntityCollection(Scene& gameScene, Layer& gameLayer);

		const Scene& GetScene() const;
		Scene& GetScene();
		const Layer& GetGameLayer() const;
		Layer& GetGameLayer();

		EntityHandle CreateEntity(const EntityHandle& entity);
		EntityHandle CreateEntity(const EntityHandle& entity, connid_t ownerConnectionId);
		EntityHandle CreateEntity(prefab_t prefabId);
		EntityHandle CreateEntity(prefab_t prefabId, connid_t ownerConnectionId);
		void RemoveAllOwnedBy(connid_t connectionId);
	};

}