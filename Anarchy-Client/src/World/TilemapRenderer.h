#pragma once
#include "Lib/World/Tilemap.h"
#include "clientpch.h"

namespace Anarchy
{

	class TilemapRenderer
	{
	private:
		Layer* m_Layer;
		Tilemap* m_Tilemap;
		float m_TileWidth;
		float m_TileHeight;

		std::vector<EntityHandle> m_Entities;

	public:
		TilemapRenderer(Layer* layer, Tilemap* tilemap, float tileWidth, float tileHeight);

		float GetTileWidth() const;
		float GetTileHeight() const;
		void SetTileWidth(float width);
		void SetTileHeight(float height);

		void Update();
		void Invalidate();
	};

}