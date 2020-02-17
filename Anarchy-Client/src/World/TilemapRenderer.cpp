#include "clientpch.h"
#include "TilemapRenderer.h"
#include "Tilemap.h"

namespace Anarchy
{

	TilemapRenderer::TilemapRenderer(Layer* layer, float tileWidth, float tileHeight)
		: m_Layer(layer), m_TileWidth(tileWidth), m_TileHeight(tileHeight), m_Entities()
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
