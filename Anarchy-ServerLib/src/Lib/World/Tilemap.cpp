#include "Tilemap.h"
#include "Logging.h"

namespace Anarchy
{

	Tilemap::View::View(int x, int y, int width, int height, int originalWidth, int originalheight, TileType* tiles)
		: m_XOffset(x), m_YOffset(y), m_Width(width), m_Height(height), m_OriginalWidth(originalWidth), m_OriginalHeight(originalheight), m_Tiles(tiles)
	{
	}

	int Tilemap::View::GetWidth() const
	{
		return m_Width;
	}

	int Tilemap::View::GetHeight() const
	{
		return m_Height;
	}

	const TileType* Tilemap::View::GetTiles() const
	{
		return m_Tiles;
	}

	bool Tilemap::View::ValidateCoordinate(int x, int y) const
	{
		return x >= 0 && y >= 0 && x < m_Width && y < m_Height;
	}

	const TileType* Tilemap::View::GetTile(int x, int y) const
	{
		return m_Tiles + (((int64_t)x + (int64_t)m_XOffset) + ((int64_t)y + (int64_t)m_YOffset) * (int64_t)m_OriginalWidth);
	}

	Tilemap::View Tilemap::View::GetTiles(int x, int y, int width, int height) const
	{
		return View(x + m_XOffset, y + m_YOffset, width, height, m_OriginalWidth, m_OriginalHeight, m_Tiles);
	}

	Tilemap::Tilemap(int width, int height)
		: m_Tiles(std::make_unique<TileType[]>((int64_t)width * (int64_t)height)), m_View(0, 0, width, height, width, height, m_Tiles.get())
	{
	}

	int Tilemap::GetWidth() const
	{
		return m_View.GetWidth();
	}

	int Tilemap::GetHeight() const
	{
		return m_View.GetHeight();
	}

	const Tilemap::View& Tilemap::GetView() const
	{
		return m_View;
	}

	void Tilemap::SetTile(int x, int y, const TileType& tile)
	{
		BLT_ASSERT(GetView().ValidateCoordinate(x, y), "Invalid coordinate");
		m_Tiles[GetIndex(x, y)] = tile;
	}

	void Tilemap::SetTiles(int x, int y, int width, int height, const TileType& tile)
	{
		BLT_ASSERT(GetView().ValidateCoordinate(x, y) && GetView().ValidateCoordinate(x + width - 1, y + height - 1), "Invalid region");
		for (int i = x; i < x + width; i++)
		{
			for (int j = y; j < y + height; j++)
			{
				SetTile(i, j, tile);
			}
		}
	}

	void Tilemap::SetTiles(int x, int y, int width, int height, const TileType* tiles)
	{
		BLT_ASSERT(GetView().ValidateCoordinate(x, y) && GetView().ValidateCoordinate(x + width - 1, y + height - 1), "Invalid region");
		for (int i = x; i < x + width; i++)
		{
			for (int j = y; j < y + height; j++)
			{
				SetTile(i, j, tiles[GetIndex(i, j)]);
			}
		}
	}

	int64_t Tilemap::GetIndex(int x, int y) const
	{
		return (int64_t)x + (int64_t)y * (int64_t)GetWidth();
	}

}
