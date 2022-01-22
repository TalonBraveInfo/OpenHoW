// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#define TERRAIN_CHUNK_ROW           16
#define TERRAIN_CHUNKS              (TERRAIN_CHUNK_ROW * TERRAIN_CHUNK_ROW)
#define TERRAIN_CHUNK_ROW_TILES     4
#define TERRAIN_CHUNK_TILES         (TERRAIN_CHUNK_ROW_TILES * TERRAIN_CHUNK_ROW_TILES)

#define TERRAIN_TILE_PIXEL_WIDTH    512
#define TERRAIN_CHUNK_PIXEL_WIDTH   2048

#define TERRAIN_ROW_TILES           (TERRAIN_CHUNK_ROW * TERRAIN_CHUNK_ROW_TILES)

#define TERRAIN_PIXEL_WIDTH         (TERRAIN_TILE_PIXEL_WIDTH * TERRAIN_ROW_TILES)

#define TERRAIN_PLAYABLE_BORDER    ( 2 * TERRAIN_CHUNK_ROW_TILES * TERRAIN_TILE_PIXEL_WIDTH )
#define TERRAIN_PLAYABLE_AREA   ( TERRAIN_PIXEL_WIDTH - ( TERRAIN_PLAYABLE_BORDER * 2 ) )

namespace ohw {
	class TextureAtlas;

	class Terrain {
	public:
		explicit Terrain( const std::string &tileset );
		~Terrain();

		struct Tile {
			/* surface properties */
			enum Surface {
				SURFACE_MUD = 0,
				SURFACE_GRASS = 1,
				SURFACE_METAL = 2,
				SURFACE_WOOD = 3,
				SURFACE_WATER = 4,
				SURFACE_STONE = 5,
				SURFACE_ROCK = 6,
				SURFACE_SAND = 7,
				SURFACE_ICE = 8,
				SURFACE_SNOW = 9,
				SURFACE_QUAGMIRE = 10,
				SURFACE_LAVA = 11,
			} surface{ SURFACE_MUD }; // e.g. wood

			enum Behaviour {
				BEHAVIOUR_NONE,
				BEHAVIOUR_WATERY = 32,
				BEHAVIOUR_MINE = 64,
				BEHAVIOUR_WALL = 128,
			} behaviour{ BEHAVIOUR_NONE }; // e.g. mine, watery

			unsigned int slip{ 0 }; // e.g. full, bottom or left?

			uint8_t texture{ 0 };

			enum Rotation {
				ROTATION_FLAG_NONE,
				ROTATION_FLAG_X = 1,
				ROTATION_FLAG_ROTATE_90 = 2,
				ROTATION_FLAG_ROTATE_180 = 4,
				ROTATION_FLAG_ROTATE_270 = 6,
			} rotation{ ROTATION_FLAG_NONE };

			float height[4]{ 0, 0, 0, 0 };
			uint8_t shading[4]{ 255, 255, 255, 255 };

			PLVector3 origin;
		};

		struct Chunk {
			Tile tiles[16];

			// We don't currently use these...
			int16_t x, y, z;

			PLGMesh *solidMesh{ nullptr };
			PLGMesh *waterMesh{ nullptr };

			/* Bounding volume encompassing the chunk. Used for determining
			 * what actors are currently within the chunk space and whether
			 * the given chunk is visible. */
			PLCollisionAABB bounds;
		};

		Chunk *GetChunk( const PLVector2 &pos );
		Tile *GetTile( float x, float y );

		float GetHeight( float x, float y );
		float GetMaxHeight() { return max_height_; }
		float GetMinHeight() { return min_height_; }

		void LoadPmg( const std::string &path );
		void LoadHeightmap( const std::string &path, int multiplier );

		PLGTexture *GetOverview() { return overview_; }

		void Serialize( const std::string &path );

		void Draw();
		void Update();

	protected:
	private:
		void GenerateChunkMesh( Chunk *chunk, const PLVector2 &offset );
		void GenerateOverview();

		float max_height_{ 0 };
		float min_height_{ 0 };

		std::vector< Chunk > chunks_;

		ohw::TextureAtlas *textureAtlas{ nullptr };
		PLGTexture *overview_{ nullptr };
	};
}
