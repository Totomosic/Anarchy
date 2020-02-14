#include "clientpch.h"
#include "TilemapRenderer.h"

namespace Anarchy
{

	TilemapRenderer::TilemapRenderer(Layer* layer, Tilemap* tilemap, float tileWidth, float tileHeight)
		: m_Layer(layer), m_Tilemap(tilemap), m_TileWidth(tileWidth), m_TileHeight(tileHeight), m_Entities()
	{
	}

	float TilemapRenderer::GetTileWidth() const
	{
		return m_TileWidth;
	}

	float TilemapRenderer::GetTileHeight() const
	{
		return m_TileHeight;
	}

	void TilemapRenderer::SetTileWidth(float width)
	{
		m_TileWidth = width;
		Invalidate();
	}

	void TilemapRenderer::SetTileHeight(float height)
	{
		m_TileHeight = height;
		Invalidate();
	}

	void TilemapRenderer::Update()
	{
		if (m_Entities.empty())
		{
			const Tilemap::View& view = m_Tilemap->GetView();
			EntityFactory factory = m_Layer->GetFactory();

			Mesh mesh;
			std::unordered_map<Color, int> materials;

			for (int i = 0; i < m_Tilemap->GetWidth(); i++)
			{
				for (int j = 0; j < m_Tilemap->GetHeight(); j++)
				{
					const TileType* tile = view.GetTile(i, j);
					Color color;
					switch (*tile)
					{
					case TileType::None:
						color = Color::Black;
						break;
					case TileType::Grass:
						color = Color(50, 200, 50);
						break;
					case TileType::Sand:
						color = Color(255, 255, 0);
						break;
					case TileType::Water:
						color = Color(0, 0, 255);
						break;
					case TileType::Stone:
						color = Color(200, 200, 200);
						break;
					default:
						color = Color::Black;
						break;
					}
					int materialIndex = 0;
					if (materials.find(color) != materials.end())
					{
						materialIndex = materials[color];
					}
					else
					{
						materialIndex = mesh.Materials.size();
						materials[color] = materialIndex;
						mesh.Materials.push_back(ResourceManager::Get().Materials().Default(color));
					}
					mesh.Models.push_back({ ResourceManager::Get().Models().Square(), Matrix4f::Translation(i * GetTileWidth(), j * GetTileHeight(), 0.0f) * Matrix4f::Scale(GetTileWidth(), GetTileHeight(), 1.0f), { materialIndex } });
				}
			}
			m_Entities.push_back(factory.CreateMesh(mesh));
		}
	}

	void TilemapRenderer::Invalidate()
	{
		for (EntityHandle& entity : m_Entities)
		{
			entity.Destroy();
		}
		m_Entities.clear();
	}

}
