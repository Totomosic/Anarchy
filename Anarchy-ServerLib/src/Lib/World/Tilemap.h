#pragma once
#include "Tile.h"
#include <memory>

namespace Anarchy
{

	class Tilemap
	{
	public:
		class View
		{
		private:
			int m_XOffset;
			int m_YOffset;
			int m_Width;
			int m_Height;
			int m_OriginalWidth;
			int m_OriginalHeight;
			Tile* m_Tiles;

		public:
			View(int x, int y, int width, int height, int originalWidth, int originalHeight, Tile* tiles);

			int GetWidth() const;
			int GetHeight() const;
			const Tile* GetTiles() const;
			bool ValidateCoordinate(int x, int y) const;

			const Tile* GetTile(int x, int y) const;
			View GetTiles(int x, int y, int width, int height) const;
		};

	private:
		std::unique_ptr<Tile[]> m_Tiles;
		Tilemap::View m_View;

	public:
		Tilemap(int width, int height);

		int GetWidth() const;
		int GetHeight() const;
		const View& GetView() const;

		void SetTile(int x, int y, const Tile& tile);
		void SetTiles(int x, int y, int width, int height, const Tile& tile);
		void SetTiles(int x, int y, int width, int height, const Tile* tiles);

	private:
		int64_t GetIndex(int x, int y) const;

	};

}