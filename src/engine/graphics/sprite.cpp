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

#include "../engine.h"
#include "sprite.h"

struct SpriteAnimation {
    unsigned int    fps{4};                 // Frames per second
    unsigned int    h_frames{1};            // Number of frames horizontally
    unsigned int    v_frames{1};            // Number of frames vertically
    unsigned int    start_x{0}, start_y{0}; // Start offsets
    unsigned int    frame_w{16};            // Width of the individual frames
    unsigned int    frame_h{16};            // Height of the individual frames
};

class SpriteAnimationSheet {
public:
    SpriteAnimationSheet(const std::string &path);
    ~SpriteAnimationSheet();

protected:
private:
    PLTexture *texture{nullptr};
    std::unordered_map<std::string, SpriteAnimation> animations;
};

class Sprite {
public:
    enum SpriteType {
        TYPE_DEFAULT,    // Depth-tested, scaled manually and oriented
    } type_{TYPE_DEFAULT};

    Sprite(SpriteType type, SpriteAnimationSheet *sheet, PLVector3 position, PLColour colour, float scale);
    ~Sprite();

    float GetScale() { return scale_; }
    void SetScale(float scale);

    PLVector3 GetPosition() { return position_; }
    void SetPosition(const PLVector3 &position);

    PLVector3 GetAngles() { return angles_; }
    void SetAngles(const PLVector3 &angles);

    PLColour GetColour() { return colour_; }
    void SetColour();

    const SpriteAnimation *GetCurrentAnimation() { return current_animation_; }
    void SetAnimation(SpriteAnimation *anim);

    void Tick();
    void Draw();

protected:
private:
    PLVector3   position_;
    PLVector3   angles_;
    PLColour    colour_{255, 255, 255, 255};
    float       scale_{1.0f};

    SpriteAnimationSheet *sheet_{nullptr};
    SpriteAnimation *current_animation_{nullptr};

    unsigned int current_frame_{0};
};

Sprite::Sprite(SpriteType type, SpriteAnimationSheet *sheet, PLVector3 position, PLColour colour, float scale) :
    type_(type),
    position_(position),
    colour_(colour),
    scale_(scale),
    sheet_(sheet) {
}

Sprite::~Sprite() = default;

void Sprite::Tick() {

}

void Sprite::Draw() {
    if(scale_ <= 0) {
        return;
    }
}

void Sprite::SetAnimation(SpriteAnimation *anim) {
    u_assert(anim != nullptr, "Invalid pointer passed for animation!\n");
    current_frame_ = 0;
    current_animation_ = anim;
}

void Sprite::SetScale(float scale) {
    scale_ = scale;
}
