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
			for (int i = 0; i < m_Tilemap->GetWidth(); i++)
			{
				for (int j = 0; j < m_Tilemap->GetHeight(); j++)
				{
					const Tile* tile = view.GetTile(i, j);
					Color color;
					switch (tile->Type)
					{
					case TileType::None:
						color = Color::Black;
						break;
					case TileType::Grass:
						color = Color::LawnGreen;
						break;
					default:
						color = Color::Black;
						break;
					}
					m_Entities.push_back(factory.Rectangle(GetTileWidth(), GetTileHeight(), color, Transform({ i * GetTileWidth(), j * GetTileHeight(), -10.0f })));
				}
			}
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
