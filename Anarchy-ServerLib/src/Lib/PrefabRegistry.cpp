#include "PrefabRegistry.h"
#include "Graphics/Resources/ResourceManager.h"

namespace Anarchy
{

	PrefabRegistry::PrefabRegistry(Layer& layer)
		: m_Layer(layer), m_Factories()
	{
		CreatePrefabs();
	}

	prefab_t PrefabRegistry::Register(const PrefabFactory& factory)
	{
		prefab_t id = GetNextPrefabId();
		m_Factories[id] = factory;
		return id;
	}

	bool PrefabRegistry::HasPrefab(prefab_t prefabId) const
	{
		return m_Factories.find(prefabId) != m_Factories.end();
	}

	const PrefabRegistry::PrefabFactory& PrefabRegistry::GetFactory(prefab_t prefabId) const
	{
		BLT_ASSERT(HasPrefab(prefabId), "No prefab with id {} exists", prefabId);
		return m_Factories.at(prefabId);
	}

	EntityHandle PrefabRegistry::CreateEntity(prefab_t prefabId, bool addGraphicsComponents) const
	{
		return GetFactory(prefabId)(m_Layer, addGraphicsComponents);
	}

	prefab_t PrefabRegistry::GetNextPrefabId() const
	{
		return (prefab_t)m_Factories.size();
	}

	void PrefabRegistry::CreatePrefabs()
	{
		Register([](Layer& layer, bool addGraphicsComponents)
			{
				EntityHandle entity = layer.GetFactory().CreateTransform(Transform());
				if (addGraphicsComponents)
				{
					Mesh mesh;
					mesh.Models.push_back({ ResourceManager::Get().Models().Square() });
					mesh.Materials.push_back(ResourceManager::Get().Materials().Default(Color::White));
					entity.Assign<Mesh>(std::move(mesh));
				}
				return entity;
			});
	}

}