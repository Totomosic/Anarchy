#include "clientpch.h"
#include "Tilemap.h"
#include "ClientState.h"

namespace Anarchy
{

	TileChunk::TileChunk(int x, int y, int width, int height, std::vector<TileType>&& tiles)
		: m_TileX(x), m_TileY(y), m_WidthTiles(width), m_HeightTiles(height), m_Tiles(std::move(tiles))
	{
	}

	int TileChunk::GetX() const
	{
		return m_TileX;
	}

	int TileChunk::GetY() const
	{
		return m_TileY;
	}

	int TileChunk::GetWidth() const
	{
		return m_WidthTiles;
	}

	int TileChunk::GetHeight() const
	{
		return m_HeightTiles;
	}

	const TileType* TileChunk::GetTiles() const
	{
		return m_Tiles.data();
	}

	TileType TileChunk::GetTile(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= GetWidth() || y >= GetHeight())
		{
			return TileType::None;
		}
		return GetTiles()[x + y * GetWidth()];
	}

	Tilemap::Tilemap(Layer* layer, int tileWidth, int tileHeight)
		: m_CachedChunks(), m_LoadedChunks(), m_ChunkWidthTiles(16), m_ChunkHeightTiles(16), m_Renderer(layer, tileWidth, tileHeight)
	{
	}

	TileType Tilemap::GetTile(int x, int y) const
	{
		TileIndex index = CalculateTileIndex(x, y);
		if (m_CachedChunks.find({ index.ChunkX, index.ChunkY }) != m_CachedChunks.end())
		{
			const std::unique_ptr<TileChunk>& chunk = m_CachedChunks.at({ index.ChunkX, index.ChunkY });
			if (chunk != nullptr)
			{
				return chunk->GetTile(index.OffsetX, index.OffsetY);
			}
		}
		return TileType::None;
	}

	void Tilemap::LoadTilePosition(int x, int y)
	{
		TileIndex index = CalculateTileIndex(x, y);
		std::vector<Vector2i> chunks;
		int radius = 1;
		for (int i = -radius; i <= radius; i++)
			for (int j = -radius; j <= radius; j++)
				chunks.push_back({ index.ChunkX + i, index.ChunkY + j });

		for (int i = m_LoadedChunks.size() - 1; i >= 0; i--)
		{
			const Vector2i& chunk = m_LoadedChunks[i];
			if (std::find(chunks.begin(), chunks.end(), chunk) == chunks.end())
			{
				UnloadChunk(chunk.x, chunk.y);
				m_LoadedChunks.erase(m_LoadedChunks.begin() + i);
			}
		}
		for (const Vector2i& chunk : chunks)
		{
			if (std::find(m_LoadedChunks.begin(), m_LoadedChunks.end(), chunk) == m_LoadedChunks.end())
			{
				m_LoadedChunks.push_back(chunk);
				if (m_CachedChunks.find(chunk) != m_CachedChunks.end())
				{
					const auto& c = m_CachedChunks[chunk];
					if (c != nullptr)
					{
						m_Renderer.DrawChunk(chunk, c.get());
					}
				}
				else
				{
					m_CachedChunks[chunk] = nullptr;
					LoadChunk(chunk.x, chunk.y).ContinueWithOnMainThread([chunk, this](std::unique_ptr<TileChunk> c)
						{
							if (c != nullptr)
							{
								m_Renderer.DrawChunk(chunk, c.get());
								m_CachedChunks[chunk] = std::move(c);
							}
							else
							{
								m_CachedChunks.erase(chunk);
							}
						});
				}
			}
		}
	}

	Tilemap::TileIndex Tilemap::CalculateTileIndex(int tileX, int tileY) const
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
		m_Renderer.DestroyChunk({ chunkX, chunkY });
	}

}