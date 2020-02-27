#include "clientpch.h"
#include "ClientEntityCollection.h"

namespace Anarchy
{

	ClientEntityCollection::ClientEntityCollection(Scene& scene, Layer& layer) : EntityCollection(scene, layer, true),
		m_ControlledEntity(InvalidNetworkId), m_Camera()
	{
	}

	bool ClientEntityCollection::IsControllingEntity(entityid_t networkId) const
	{
		return networkId == m_ControlledEntity;
	}

	EntityHandle ClientEntityCollection::GetCamera() const
	{
		return m_Camera;
	}

	void ClientEntityCollection::SetControlledEntity(entityid_t networkId)
	{
		m_ControlledEntity = networkId;
		if (m_ControlledEntity != InvalidNetworkId && m_Camera)
		{
			m_Camera.GetTransform()->SetParent(GetEntityByNetworkId(m_ControlledEntity).GetTransform().Get());
		}
		else if (m_Camera)
		{
			m_Camera.GetTransform()->SetParent(nullptr);
		}
	}

	void ClientEntityCollection::SetCamera(const EntityHandle& camera)
	{
		m_Camera = camera;
		if (m_ControlledEntity != InvalidNetworkId)
		{
			m_Camera.GetTransform()->SetParent(GetEntityByNetworkId(m_ControlledEntity).GetTransform().Get());
		}
	}

	EntityHandle ClientEntityCollection::CreateFromEntityState(const EntityState& state)
	{
		EntityHandle entity = EntityCollection::CreateFromEntityState(state);
		if (!state.Name.empty() && entity.HasComponent<Mesh>())
		{
			ComponentHandle mesh = entity.GetComponent<Mesh>();
			int materialIndex = (int)mesh->Materials.size();

			float fontSize = 16;
			float scalingFactor = 0.5f;
			float scaling = scalingFactor / fontSize;

			ResourcePtr<Font> font = ResourceManager::Get().Fonts().Arial(fontSize);
			mesh->Models.push_back({ new Model(TextFactory(state.Name, font)), Matrix4f::Scale(scaling, scaling, 1.0f) * Matrix4f::Translation(0, fontSize / scalingFactor, 0), { materialIndex } });
			mesh->Materials.push_back(ResourceManager::Get().Materials().Font(font, Color::Black));
		}
		return entity;
	}

}