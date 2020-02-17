#include "clientpch.h"
#include "Tilemap.h"
#include "ClientState.h"

namespace Anarchy
{

	TileChunk::TileChunk(int x, int y, int width, int height, std::vector<TileType>&& tiles)
		: m_ChunkX(x), m_ChunkY(y), m_WidthTiles(width), m_HeightTiles(height), m_Tiles(std::move(tiles))
	{
	}

	Tilemap::Tilemap(Layer* layer, int tileWidth, int tileHeight)
		: m_LoadedChunks(), m_ChunkWidthTiles(32), m_ChunkHeightTiles(32), m_Renderer(layer, tileWidth, tileHeight)
	{
	}

	void Tilemap::LoadTilePosition(int64_t x, int64_t y)
	{
		TileIndex index = CalculateTileIndex(x, y);
		std::vector<Vector2i> chunks;
		chunks.push_back({ index.ChunkX - 1, index.ChunkY - 1 });
		chunks.push_back({ index.ChunkX + 0, index.ChunkY - 1 });
		chunks.push_back({ index.ChunkX + 1, index.ChunkY - 1 });
		chunks.push_back({ index.ChunkX - 1, index.ChunkY + 0 });
		chunks.push_back({ index.ChunkX + 0, index.ChunkY + 0 });
		chunks.push_back({ index.ChunkX + 1, index.ChunkY + 0 });
		chunks.push_back({ index.ChunkX - 1, index.ChunkY + 1 });
		chunks.push_back({ index.ChunkX + 0, index.ChunkY + 1 });
		chunks.push_back({ index.ChunkX + 1, index.ChunkY + 1 });
		auto it = m_LoadedChunks.begin();
		while (it != m_LoadedChunks.end())
		{
			if (std::find(chunks.begin(), chunks.end(), it->first) == chunks.end())
			{
				it = m_LoadedChunks.erase(it);
			}
			else
			{
				it++;
			}
		}
		for (const Vector2i& chunk : chunks)
		{
			if (m_LoadedChunks.find(chunk) == m_LoadedChunks.end())
			{
				m_LoadedChunks[chunk] = nullptr;
				LoadChunk(chunk.x, chunk.y).ContinueWithOnMainThread([this, chunk](std::unique_ptr<TileChunk> result)
					{
						m_LoadedChunks[chunk] = std::move(result);
					});
			}
		}
	}

	Tilemap::TileIndex Tilemap::CalculateTileIndex(int64_t tileX, int64_t tileY) const
	{
		TileIndex index;
		index.ChunkX = tileX / m_ChunkWidthTiles;
		index.ChunkY = tileY / m_ChunkHeightTiles;
		index.OffsetX = tileX - index.ChunkX * m_ChunkWidthTiles;
		index.OffsetY = tileY - index.ChunkY * m_ChunkHeightTiles;
		return index;
	}

	Tilemap::ChunkBounds Tilemap::GetChunkBounds(int chunkX, int chunkY) const
	{
		ChunkBounds bounds;
		bounds.x = chunkX * m_ChunkWidthTiles;
		bounds.y = chunkY * m_ChunkHeightTiles;
		bounds.Width = m_ChunkWidthTiles;
		bounds.Height = m_ChunkHeightTiles;
		return bounds;
	}

	Task<std::unique_ptr<TileChunk>> Tilemap::LoadChunk(int chunkX, int chunkY) const
	{
		ChunkBounds bounds = GetChunkBounds(chunkX, chunkY);
		GetTilemapRequest request;
		request.DimensionId = 0;
		request.x = bounds.x;
		request.y = bounds.y;
		request.Width = bounds.Width;
		request.Height = bounds.Height;
		return ClientState::Get().GetConnection().GetSocketApi().GetTilemap(request, 5.0).ContinueWith([](std::optional<GetTilemapResponse> response) -> std::unique_ptr<TileChunk>
			{
				if (response)
				{
					return std::make_unique<TileChunk>(response->x, response->y, response->Width, response->Height, std::move(response->Tiles));
				}
				return nullptr;
			});
	}

	void Tilemap::UnloadChunk(int chunkX, int chunkY)
	{
	}

}