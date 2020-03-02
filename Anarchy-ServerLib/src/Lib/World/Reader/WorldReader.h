#pragma once
#include "Core/Utils/Filesystem/Filesystem.h"
#include "../Tile.h"

namespace Anarchy
{

	class TileView
	{
	private:
		std::unique_ptr<TileType[]> m_Tiles;
		uint32_t m_Width;
		uint32_t m_Height;

	public:
		TileView(uint32_t width, uint32_t height, std::unique_ptr<TileType[]>&& tiles);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		TileType* GetTiles() const;
		size_t GetIndex(uint32_t x, uint32_t y) const;
		TileType& GetTile(uint32_t x, uint32_t y) const;
	};

	class WorldReader
	{
	private:
		struct TileIndex
		{
		public:
			uint16_t xChunk;
			uint16_t yChunk;
			uint16_t xOffset;
			uint16_t yOffset;
		};

	private:
		uint32_t m_WidthTiles;
		uint32_t m_HeightTiles;
		uint16_t m_WidthChunks;
		uint16_t m_HeightChunks;
		uint16_t m_XTilesPerChunk;
		uint16_t m_YTilesPerChunk;

		Bolt::DirectoryPath m_ChunkDirectory;
		std::vector<Bolt::FilePath> m_Filenames;

		size_t m_MaxSpaceAllowed;
		mutable std::unordered_map<uint32_t, std::unique_ptr<TileType[]>> m_LoadedChunks;
		mutable std::vector<uint32_t> m_RecentlyUsed;

	public:
		WorldReader(const Bolt::DirectoryPath& worldDirectory, size_t maxSpaceAllowed = 1 * 1024 * 1024 * 1024);
		~WorldReader();

		uint32_t GetWidthInTiles() const;
		uint32_t GetHeightInTiles() const;
		uint16_t GetWidthInChunks() const;
		uint16_t GetHeightInChunks() const;
		uint16_t GetXTilesPerChunk() const;
		uint16_t GetYTilesPerChunk() const;

		TileType GetTile(int64_t x, int64_t y) const;
		TileView GetTiles(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const;
		
		void SetTile(uint32_t x, uint32_t y, TileType tile);
		void SetTiles(uint32_t x, uint32_t y, uint32_t width, uint32_t height, TileType tile);
		void SetTiles(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const TileType* tiles);
		void Commit();

	private:
		size_t CalculateChunkByteSize() const;
		size_t CalculateTotalSpaceUsed() const;
		uint32_t CalculateTilesPerChunk() const;
		TileIndex CreateTileIndex(uint32_t tileX, uint32_t tileY) const;

		uint32_t GetChunkIndex(uint16_t chunkX, uint16_t chunkY) const;
		uint32_t GetTileOffsetIndex(uint16_t tileXOffset, uint16_t tileYOffset) const;

		TileType* LoadChunk(uint32_t chunkX, uint32_t chunkY) const;

		void UpdateChunk(uint16_t chunkX, uint16_t chunkY, const TileType* data);
		void UpdateChunk(uint32_t chunkIndex, const TileType* data);
		void UseChunk(uint32_t chunkIndex) const;
		void PurgeLeastUsedChunk();

	};

}