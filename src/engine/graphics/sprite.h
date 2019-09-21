/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

struct SpriteAnimation {
  unsigned int fps{4};                 // Frames per second
  unsigned int h_frames{1};            // Number of frames horizontally
  unsigned int v_frames{1};            // Number of frames vertically
  unsigned int start_x{0}, start_y{0}; // Start offsets
  unsigned int frame_w{16};            // Width of the individual frames
  unsigned int frame_h{16};            // Height of the individual frames
};

class SpriteTextureSheet {
 public:
  explicit SpriteTextureSheet(const std::string& path);
  ~SpriteTextureSheet();

  const SpriteAnimation* GetAnimation(const std::string& name);

 protected:
 private:
  PLTexture* texture_{nullptr};
  std::unordered_map<std::string, SpriteAnimation> animations_;
};

class Sprite {
 public:
  enum SpriteType {
    TYPE_DEFAULT,    // Depth-tested, scaled manually and oriented
  } type_{TYPE_DEFAULT};

  Sprite(SpriteType type, SpriteTextureSheet* sheet, PLColour colour, float scale);
  ~Sprite();

  float GetScale() { return scale_; }
  void SetScale(float scale);

  PLVector3 GetPosition() { return position_; }
  void SetPosition(const PLVector3& position);

  PLVector3 GetAngles() { return angles_; }
  void SetAngles(const PLVector3& angles);

  PLColour GetColour() { return colour_; }
  void SetColour(const PLColour& colour);

  const SpriteAnimation* GetCurrentAnimation() { return current_animation_; }
  void SetAnimation(SpriteAnimation* anim);

  void Tick();
  void Draw();

 protected:
 private:
  PLVector3 position_;
  PLVector3 angles_;
  PLColour colour_{255, 255, 255, 255};
  float scale_{1.0f};

  SpriteTextureSheet* sheet_{nullptr};
  SpriteAnimation* current_animation_{nullptr};

  unsigned int current_frame_{0};
  double frame_delay_{0};
};
