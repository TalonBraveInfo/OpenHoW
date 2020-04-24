/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define TERRAIN_CHUNK_ROW           16
#define TERRAIN_CHUNKS              (TERRAIN_CHUNK_ROW * TERRAIN_CHUNK_ROW)
#define TERRAIN_CHUNK_ROW_TILES     4
#define TERRAIN_CHUNK_TILES         (TERRAIN_CHUNK_ROW_TILES * TERRAIN_CHUNK_ROW_TILES)

#define TERRAIN_TILE_PIXEL_WIDTH    512
#define TERRAIN_CHUNK_PIXEL_WIDTH   2048

#define TERRAIN_ROW_TILES           (TERRAIN_CHUNK_ROW * TERRAIN_CHUNK_ROW_TILES)

#define TERRAIN_PIXEL_WIDTH         (TERRAIN_TILE_PIXEL_WIDTH * TERRAIN_ROW_TILES)

class TextureAtlas;

class Terrain {
 public:
  explicit Terrain(const std::string& tileset);
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
    } surface{SURFACE_MUD}; // e.g. wood

    enum Behaviour {
      BEHAVIOUR_NONE,
      BEHAVIOUR_WATERY = 32,
      BEHAVIOUR_MINE = 64,
      BEHAVIOUR_WALL = 128,
    } behaviour{BEHAVIOUR_NONE}; // e.g. mine, watery

    unsigned int slip{0}; // e.g. full, bottom or left?

    uint8_t texture{0};

    enum Rotation {
      ROTATION_FLAG_NONE,
      ROTATION_FLAG_X = 1,
      ROTATION_FLAG_ROTATE_90 = 2,
      ROTATION_FLAG_ROTATE_180 = 4,
      ROTATION_FLAG_ROTATE_270 = 6,
    } rotation{ROTATION_FLAG_NONE};

    float height[4]{0, 0, 0, 0};
    uint8_t shading[4]{255, 255, 255, 255};
  };

  struct Chunk {
    Tile tiles[16];
    PLModel* model{nullptr};
  };

  Chunk* GetChunk(const PLVector2& pos);
  Tile* GetTile(const PLVector2& pos);

  float GetHeight(const PLVector2& pos);
  float GetMaxHeight() { return max_height_; }
  float GetMinHeight() { return min_height_; }

  void LoadPmg(const std::string& path);
  void LoadHeightmap(const std::string& path, int multiplier);

  PLTexture* GetOverview() { return overview_; }

  void Serialize(const std::string& path);

  void Draw();
  void Update();

 protected:
 private:
  void GenerateModel(Chunk* chunk, const PLVector2& offset);
  void GenerateOverview();

  float max_height_{0};
  float min_height_{0};

  std::vector<Chunk> chunks_;

  TextureAtlas* atlas_{nullptr};
  PLTexture* overview_{nullptr};
};
