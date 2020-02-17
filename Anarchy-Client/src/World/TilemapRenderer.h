#pragma once
#include "clientpch.h"
#include "Lib/World/Tile.h"

namespace Anarchy
{

	class Tilemap;

	class TilemapRenderer
	{
	private:
		Layer* m_Layer;
		float m_TileWidth;
		float m_TileHeight;

		std::vector<EntityHandle> m_Entities;

	public:
		TilemapRenderer(Layer* layer, float tileWidth, float tileHeight);

		float GetTileWidth() const;
		float GetTileHeight() const;
		void SetTileWidth(float width);
		void SetTileHeight(float height);

		void Update();
		void Invalidate();
	};

}