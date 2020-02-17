#pragma once
#include "TilemapRenderer.h"

namespace std
{

	template<>
	struct hash<Vector2i>
	{
	public:
		size_t operator()(const Vector2i& value) const
		{
			size_t result = (size_t)value.x;
			result |= ((size_t)value.y) << 32;
			return result;
		}
	};

}

namespace Anarchy
{

	class TileChunk
	{
	private:
		int m_ChunkX;
		int m_ChunkY;
		int m_WidthTiles;
		int m_HeightTiles;
		std::vector<TileType> m_Tiles;

	public:
		TileChunk(int x, int y, int width, int height, std::vector<TileType>&& tiles);
	};

	class Tilemap
	{
	private:
		struct TileIndex
		{
		public:
			int ChunkX;
			int ChunkY;
			int OffsetX;
			int OffsetY;
		};

		struct ChunkBounds
		{
		public:
			int64_t x;
			int64_t y;
			int Width;
			int Height;
		};

	private:
		std::unordered_map<Vector2i, std::unique_ptr<TileChunk>> m_LoadedChunks;
		int m_ChunkWidthTiles;
		int m_ChunkHeightTiles;
		TilemapRenderer m_Renderer;

	public:
		Tilemap(Layer* layer, int tileWidth, int tileHeight);

		void LoadTilePosition(int64_t x, int64_t y);

	private:
		TileIndex CalculateTileIndex(int64_t tileX, int64_t tileY) const;
		ChunkBounds GetChunkBounds(int chunkX, int chunkY) const;
		Task<std::unique_ptr<TileChunk>> LoadChunk(int chunkX, int chunkY) const;
		void UnloadChunk(int chunkX, int chunkY);

	};

}