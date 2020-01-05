#pragma once
#include "Engine/Scene/Layer.h"
#include "ServerLib.h"

namespace Anarchy
{

	class PrefabRegistry
	{
	public:
		// boolean is for whether or not to add graphics components such as a Mesh
		using PrefabFactory = std::function<EntityHandle(Layer&, bool)>;

	private:
		Layer& m_Layer;
		std::unordered_map<prefab_t, PrefabFactory> m_Factories;

	public:
		PrefabRegistry(Layer& layer);

		prefab_t Register(const PrefabFactory& factory);

		bool HasPrefab(prefab_t prefabId) const;
		const PrefabFactory& GetFactory(prefab_t prefabId) const;
		EntityHandle CreateEntity(prefab_t prefabId, bool addGraphicsComponents) const;

	private:
		prefab_t GetNextPrefabId() const;
		void CreatePrefabs();

	};

}