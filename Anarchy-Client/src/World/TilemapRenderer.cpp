#include "clientpch.h"
#include "TilemapRenderer.h"
#include "Tilemap.h"

namespace Anarchy
{

	TilemapRenderer::TilemapRenderer(Layer* layer, float tileWidth, float tileHeight)
		: m_Layer(layer), m_TileWidth(tileWidth), m_TileHeight(tileHeight), m_Chunks()
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

	void TilemapRenderer::DrawChunk(const Vector2i& chunkIndex, const TileChunk* chunk)
	{
		Mesh mesh;
		std::unordered_map<Color, int> materialIndices;
		for (int x = 0; x < chunk->GetWidth(); x++)
		{
			for (int y = 0; y < chunk->GetHeight(); y++)
			{
				int index = x + y * chunk->GetWidth();
				TileType tile = chunk->GetTiles()[index];
				Color c;
				switch (tile)
				{
				case TileType::Grass:
					c = Color(20, 160, 25);
					break;
				case TileType::Sand:
					c = Color(255, 225, 50);
					break;
				case TileType::Water:
					c = Color(0, 20, 150);
					break;
				default:
					c = Color::Black;
					break;
				}
				int materialIndex = 0;
				if (materialIndices.find(c) != materialIndices.end())
				{
					materialIndex = materialIndices[c];
				}
				else
				{
					materialIndex = mesh.Materials.size();
					mesh.Materials.push_back(ResourceManager::Get().Materials().Default(c));
					materialIndices[c] = materialIndex;
				}
				mesh.Models.push_back({ ResourceManager::Get().Models().Square(), Matrix4f::Translation(x, y, 0), { materialIndex } });
			}
		}
		float xoffset = (chunk->GetX());// *GetTileWidth();
		float yoffset = (chunk->GetY());// *GetTileHeight();
		EntityHandle entity = m_Layer->GetFactory().CreateMesh(std::move(mesh), Transform({ xoffset, yoffset, 0.0f }));
		m_Chunks[chunkIndex] = { chunk, entity };
	}

	void TilemapRenderer::DestroyChunk(const Vector2i& chunk)
	{
		if (m_Chunks.find(chunk) != m_Chunks.end())
		{
			EntityHandle entity = m_Chunks[chunk].Entity;
			if (entity)
			{
				entity.Destroy();
			}
			m_Chunks.erase(chunk);
		}
	}

	void TilemapRenderer::Invalidate()
	{
		
	}

}
