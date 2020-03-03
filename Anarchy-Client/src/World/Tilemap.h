#pragma once
#include "TilemapRenderer.h"

namespace Anarchy
{

	class TileChunk
	{
	private:
		int m_TileX;
		int m_TileY;
		int m_WidthTiles;
		int m_HeightTiles;
		std::vector<TileType> m_Tiles;

	public:
		TileChunk(int x, int y, int width, int height, std::vector<TileType>&& tiles);

		int GetX() const;
		int GetY() const;
		int GetWidth() const;
		int GetHeight() const;
		const TileType* GetTiles() const;
		TileType GetTile(int x, int y) const;
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
			int x;
			int y;
			int Width;
			int Height;
		};

	private:
		std::unordered_map<Vector2i, std::unique_ptr<TileChunk>> m_CachedChunks;
		std::vector<Vector2i> m_LoadedChunks;
		int m_ChunkWidthTiles;
		int m_ChunkHeightTiles;
		TilemapRenderer m_Renderer;

	public:
		Tilemap(Layer* layer, int tileWidth, int tileHeight);

		TileType GetTile(int x, int y) const;
		void LoadTilePosition(int x, int y);

	private:
		TileIndex CalculateTileIndex(int tileX, int tileY) const;
		ChunkBounds GetChunkBounds(int chunkX, int chunkY) const;
		Task<std::unique_ptr<TileChunk>> LoadChunk(int chunkX, int chunkY) const;
		void UnloadChunk(int chunkX, int chunkY);

	};

}