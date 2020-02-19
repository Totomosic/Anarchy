#pragma once
#include "clientpch.h"
#include "Lib/World/Tile.h"

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

	class Tilemap;
	class TileChunk;

	class TilemapRenderer
	{
	private:
		struct RenderableChunk
		{
		public:
			const TileChunk* Chunk;
			EntityHandle Entity;
		};

	private:
		Layer* m_Layer;
		float m_TileWidth;
		float m_TileHeight;

		std::unordered_map<Vector2i, RenderableChunk> m_Chunks;

	public:
		TilemapRenderer(Layer* layer, float tileWidth, float tileHeight);

		float GetTileWidth() const;
		float GetTileHeight() const;
		void SetTileWidth(float width);
		void SetTileHeight(float height);

		void DrawChunk(const Vector2i& chunkIndex, const TileChunk* chunk);
		void DestroyChunk(const Vector2i& chunk);

		void Invalidate();
	};

}