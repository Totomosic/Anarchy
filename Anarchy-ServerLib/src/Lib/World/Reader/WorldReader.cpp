#include "WorldReader.h"

namespace Anarchy
{

	TileView::TileView(uint32_t width, uint32_t height, std::unique_ptr<TileType[]>&& tiles)
		: m_Tiles(std::move(tiles)), m_Width(width), m_Height(height)
	{
	}

	uint32_t TileView::GetWidth() const
	{
		return m_Width;
	}

	uint32_t TileView::GetHeight() const
	{
		return m_Height;
	}

	TileType* TileView::GetTiles() const
	{
		return m_Tiles.get();
	}

	size_t TileView::GetIndex(uint32_t x, uint32_t y) const
	{
		return (size_t)(x) + (size_t)y * (size_t)GetWidth();
	}

	TileType& TileView::GetTile(uint32_t x, uint32_t y) const
	{
		return m_Tiles[GetIndex(x, y)];
	}


	WorldReader::WorldReader(const Bolt::DirectoryPath& worldDirectory, size_t maxSpaceAllowed)
		: m_WidthTiles(0), m_HeightTiles(0), m_WidthChunks(0), m_HeightChunks(0), m_XTilesPerChunk(0), m_YTilesPerChunk(0), m_ChunkDirectory(), m_Filenames(), m_MaxSpaceAllowed(maxSpaceAllowed), m_LoadedChunks(), m_RecentlyUsed()
	{
		Bolt::FilePath worldFilename = Bolt::FilePath::Combine(worldDirectory, "world.dat");
		Bolt::File worldFile = Bolt::Filesystem::Open(worldFilename, Bolt::OpenMode::Read);
		size_t size = worldFile.GetSize();
		char* buffer = new char[size];
		worldFile.Read(buffer, size);

		m_WidthTiles = *(uint32_t*)(buffer + sizeof(uint32_t) * 0 + sizeof(uint16_t) * 0 + sizeof(uint8_t) * 0);
		m_HeightTiles =	*(uint32_t*)(buffer + sizeof(uint32_t) * 1 + sizeof(uint16_t) * 0 + sizeof(uint8_t) * 0);

		m_WidthChunks =	*(uint16_t*)(buffer + sizeof(uint32_t) * 2 + sizeof(uint16_t) * 0 + sizeof(uint8_t) * 0);
		m_HeightChunks = *(uint16_t*)(buffer + sizeof(uint32_t) * 2 + sizeof(uint16_t) * 1 + sizeof(uint8_t) * 0);
		m_XTilesPerChunk = *(uint16_t*)(buffer + sizeof(uint32_t) * 2 + sizeof(uint16_t) * 2 + sizeof(uint8_t) * 0);
		m_YTilesPerChunk = *(uint16_t*)(buffer + sizeof(uint32_t) * 2 + sizeof(uint16_t) * 3 + sizeof(uint8_t) * 0);

		uint8_t filenameLength = *(uint8_t*)(buffer + sizeof(uint32_t) * 2 + sizeof(uint16_t) * 4 + sizeof(uint8_t) * 0);
		size_t startIndex = sizeof(uint32_t) * 2 + sizeof(uint16_t) * 4 + sizeof(uint8_t) * 1;
		
		Bolt::DirectoryPath chunkDirectory = Bolt::DirectoryPath::Combine(worldDirectory, std::string(buffer + startIndex));
		startIndex += filenameLength;
		for (uint32_t i = 0; i < m_WidthChunks * m_HeightChunks; i++)
		{
			m_Filenames.push_back(Bolt::FilePath::Combine(chunkDirectory, std::string(buffer + startIndex)));
			startIndex += filenameLength;
		}
		delete[] buffer;
	}

	WorldReader::~WorldReader()
	{
		Commit();
	}

	uint32_t WorldReader::GetWidthInTiles() const
	{
		return m_WidthTiles;
	}

	uint32_t WorldReader::GetHeightInTiles() const
	{
		return m_HeightTiles;
	}

	uint16_t WorldReader::GetWidthInChunks() const
	{
		return m_WidthChunks;
	}

	uint16_t WorldReader::GetHeightInChunks() const
	{
		return m_HeightChunks;
	}

	uint16_t WorldReader::GetXTilesPerChunk() const
	{
		return m_XTilesPerChunk;
	}

	uint16_t WorldReader::GetYTilesPerChunk() const
	{
		return m_YTilesPerChunk;
	}

	TileType WorldReader::GetTile(uint32_t x, uint32_t y) const
	{
		TileIndex index = CreateTileIndex(x, y);
		TileType* data = LoadChunk(index.xChunk, index.yChunk);
		uint32_t dataIndex = GetTileOffsetIndex(index.xOffset, index.yOffset);
		return data[dataIndex];
	}

	TileView WorldReader::GetTiles(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
	{
		TileIndex minIndex = CreateTileIndex(x, y);
		TileIndex maxIndex = CreateTileIndex(x + width - 1, y + height - 1);
		std::unique_ptr<TileType[]> result = std::make_unique<TileType[]>((size_t)width * (size_t)height);

		uint16_t currentX = 0;
		uint16_t currentY = 0;

		for (uint16_t yChunk = minIndex.yChunk; yChunk <= maxIndex.yChunk; yChunk++)
		{
			uint16_t baseY = currentY;
			currentX = 0;
			for (uint16_t xChunk = minIndex.xChunk; xChunk <= maxIndex.xChunk; xChunk++)
			{
				uint16_t xOffset = 0;
				uint16_t yOffset = 0;
				uint16_t xEnd = GetXTilesPerChunk() - 1;
				uint16_t yEnd = GetYTilesPerChunk() - 1;
				if (xChunk == minIndex.xChunk)
					xOffset = minIndex.xOffset;
				if (xChunk == maxIndex.xChunk)
					xEnd = maxIndex.xOffset;
				if (yChunk == minIndex.yChunk)
					yOffset = minIndex.yOffset;
				if (yChunk == maxIndex.yChunk)
					yEnd = maxIndex.yOffset;
				TileType* data = LoadChunk(xChunk, yChunk);
				uint16_t baseX = currentX;
				uint16_t rowIndex = 0;
				for (uint16_t row = yOffset; row <= yEnd; row++)
				{
					uint32_t index = xOffset + row * GetXTilesPerChunk();
					uint32_t rowWidth = xEnd - xOffset + 1;
					memcpy(result.get() + ((size_t)baseX + ((size_t)baseY + (size_t)rowIndex) * (size_t)width), data + index, rowWidth * sizeof(TileType));
					currentX = baseX + rowWidth;
					rowIndex += 1;
				}
				currentY = baseY + rowIndex;
			}
		}
		return TileView(width, height, std::move(result));
	}

	void WorldReader::SetTile(uint32_t x, uint32_t y, TileType tile)
	{
		TileIndex index = CreateTileIndex(x, y);
		TileType* data = LoadChunk(index.xChunk, index.yChunk);
		uint32_t dataIndex = GetTileOffsetIndex(index.xOffset, index.yOffset);
		data[dataIndex] = tile;
	}

	void WorldReader::SetTiles(uint32_t x, uint32_t y, uint32_t width, uint32_t height, TileType tile)
	{
		BLT_ASSERT(false, "Not implemented");
	}

	void WorldReader::SetTiles(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const TileType* tiles)
	{
		TileIndex minIndex = CreateTileIndex(x, y);
		TileIndex maxIndex = CreateTileIndex(x + width - 1, y + height - 1);

		uint16_t currentX = 0;
		uint16_t currentY = 0;

		for (uint16_t yChunk = minIndex.yChunk; yChunk <= maxIndex.yChunk; yChunk++)
		{
			uint16_t baseY = currentY;
			currentX = 0;
			for (uint16_t xChunk = minIndex.xChunk; xChunk <= maxIndex.xChunk; xChunk++)
			{
				uint16_t xOffset = 0;
				uint16_t yOffset = 0;
				uint16_t xEnd = GetXTilesPerChunk() - 1;
				uint16_t yEnd = GetYTilesPerChunk() - 1;
				if (xChunk == minIndex.xChunk)
					xOffset = minIndex.xOffset;
				if (xChunk == maxIndex.xChunk)
					xEnd = maxIndex.xOffset;
				if (yChunk == minIndex.yChunk)
					yOffset = minIndex.yOffset;
				if (yChunk == maxIndex.yChunk)
					yEnd = maxIndex.yOffset;
				TileType* chunkData = LoadChunk(xChunk, yChunk);
				uint16_t baseX = currentX;
				uint16_t rowIndex = 0;
				for (uint16_t row = yOffset; row <= yEnd; row++)
				{
					uint32_t index = xOffset + row * GetXTilesPerChunk();
					uint32_t rowWidth = xEnd - xOffset + 1;
					size_t dataIndex = (size_t)baseX + ((size_t)baseY + (size_t)rowWidth) * (size_t)width;
					memcpy(chunkData + (size_t)index, tiles + (size_t)dataIndex, rowWidth * sizeof(TileType));
					currentX = baseX + rowWidth;
					rowIndex += 1;
				}
				currentY = baseY + rowIndex;
			}
		}
	}

	void WorldReader::Commit()
	{
		for (const auto& pair : m_LoadedChunks)
		{
			UpdateChunk(pair.first, pair.second.get());
		}
		m_LoadedChunks.clear();
		m_RecentlyUsed.clear();
	}

	size_t WorldReader::CalculateChunkByteSize() const
	{
		return (size_t)GetXTilesPerChunk() * (size_t)GetYTilesPerChunk() * sizeof(TileType);
	}

	size_t WorldReader::CalculateTotalSpaceUsed() const
	{
		return m_LoadedChunks.size() * CalculateChunkByteSize();
	}

	uint32_t WorldReader::CalculateTilesPerChunk() const
	{
		return (uint32_t)GetXTilesPerChunk() * (uint32_t)GetYTilesPerChunk();
	}

	WorldReader::TileIndex WorldReader::CreateTileIndex(uint32_t tileX, uint32_t tileY) const
	{
		TileIndex index;
		index.xChunk = (uint16_t)(tileX / GetXTilesPerChunk());
		index.yChunk = (uint16_t)(tileY / GetYTilesPerChunk());
		index.xOffset = tileX - index.xChunk * GetXTilesPerChunk();
		index.yOffset = tileY - index.yChunk * GetYTilesPerChunk();
		return index;
	}

	uint32_t WorldReader::GetChunkIndex(uint16_t chunkX, uint16_t chunkY) const
	{
		return (uint32_t)chunkX + (uint32_t)chunkY * (uint32_t)GetWidthInChunks();
	}

	uint32_t WorldReader::GetTileOffsetIndex(uint16_t tileXOffset, uint16_t tileYOffset) const
	{
		return (uint32_t)tileXOffset + (uint32_t)tileYOffset * (uint32_t)GetXTilesPerChunk();
	}

	TileType* WorldReader::LoadChunk(uint32_t chunkX, uint32_t chunkY) const
	{
		uint32_t chunkIndex = GetChunkIndex(chunkX, chunkY);
		auto it = m_LoadedChunks.find(chunkIndex);
		if (it == m_LoadedChunks.end())
		{
			const Bolt::FilePath& filename = m_Filenames[chunkIndex];
			Bolt::File file = Bolt::Filesystem::Open(filename, Bolt::OpenMode::Read);
			std::unique_ptr<TileType[]> tileData = std::make_unique<TileType[]>(CalculateTilesPerChunk());
			file.Read(tileData.get(), CalculateChunkByteSize());
			m_LoadedChunks[chunkIndex] = std::move(tileData);
		}
		UseChunk(chunkIndex);
		return m_LoadedChunks[chunkIndex].get();
	}

	void WorldReader::UpdateChunk(uint16_t chunkX, uint16_t chunkY, const TileType* data)
	{
		uint32_t chunkIndex = GetChunkIndex(chunkX, chunkY);
		UpdateChunk(chunkIndex, data);
	}

	void WorldReader::UpdateChunk(uint32_t chunkIndex, const TileType* data)
	{
		auto it = m_LoadedChunks.find(chunkIndex);
		if (it != m_LoadedChunks.end())
		{
			memcpy(it->second.get(), data, CalculateChunkByteSize());
		}
		const Bolt::FilePath& filename = m_Filenames[chunkIndex];
		Bolt::File f = Bolt::Filesystem::Open(filename, Bolt::OpenMode::Write);
		f.Write(data, CalculateChunkByteSize());
		UseChunk(chunkIndex);
	}

	void WorldReader::UseChunk(uint32_t chunkIndex) const
	{
		if (m_LoadedChunks.find(chunkIndex) != m_LoadedChunks.end())
		{
			auto it = std::find(m_RecentlyUsed.begin(), m_RecentlyUsed.end(), chunkIndex);
			if (it != m_RecentlyUsed.end())
			{
				m_RecentlyUsed.erase(it);
			}
			m_RecentlyUsed.push_back(chunkIndex);
		}
	}

	void WorldReader::PurgeLeastUsedChunk()
	{
		if (m_RecentlyUsed.size() > 0)
		{
			uint32_t index = m_RecentlyUsed[0];
			UpdateChunk(index, m_LoadedChunks[index].get());
			m_LoadedChunks.erase(index);
			m_RecentlyUsed.erase(m_RecentlyUsed.begin());
		}
	}

}