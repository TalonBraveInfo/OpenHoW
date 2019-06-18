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
#include "../script/ScriptConfig.h"
#include "sprite.h"
#include "display.h"

/************************************************************/
/* Sprite Implementation
 * A lot of this is slightly expanded to incorporate
 * functionality to be utilised in the future or for
 * other projects. OpenHoW probably won't be depending
 * on the animation implementation for example...
 * */

struct SpriteAnimation {
    unsigned int    fps{4};                 // Frames per second
    unsigned int    h_frames{1};            // Number of frames horizontally
    unsigned int    v_frames{1};            // Number of frames vertically
    unsigned int    start_x{0}, start_y{0}; // Start offsets
    unsigned int    frame_w{16};            // Width of the individual frames
    unsigned int    frame_h{16};            // Height of the individual frames
};

class SpriteTextureSheet {
public:
    explicit SpriteTextureSheet(const std::string &path);
    ~SpriteTextureSheet();

    const SpriteAnimation *GetAnimation(const std::string &name);

protected:
private:
    PLTexture *texture_{nullptr};
    std::unordered_map<std::string, SpriteAnimation> animations_;
};

SpriteTextureSheet::SpriteTextureSheet(const std::string &path) {
    texture_ = Display_GetDefaultTexture();

    try {
        ScriptConfig sheet_data(path);

        std::string filter = sheet_data.GetStringProperty("filter");
        PLTextureFilter fmode = PL_TEXTURE_FILTER_MIPMAP_LINEAR;
        if(filter == "linear") {
            fmode = PL_TEXTURE_FILTER_MIPMAP_LINEAR;
        } else if(filter == "nearest") {
            fmode = PL_TEXTURE_FILTER_MIPMAP_NEAREST;
        } else if(filter == "nomip linear") {
            fmode = PL_TEXTURE_FILTER_LINEAR;
        } else if(filter == "nomip nearest") {
            fmode = PL_TEXTURE_FILTER_NEAREST;
        } else if(!filter.empty()) {
            LogWarn("Invalid filter mode \"%s\", ignoring!\n", filter.c_str());
        }

        std::string texture_path = sheet_data.GetStringProperty("texture", "", false);
        if(!texture_path.empty()) {
            texture_ = Display_LoadTexture(texture_path.c_str(), fmode);
        }
    } catch(const std::exception &e) {
        LogWarn("Failed to load texture sheet data, using defaults...\n");
    }
}

const SpriteAnimation *SpriteTextureSheet::GetAnimation(const std::string &name) {
    auto animation = animations_.find(name);
    if(animation != animations_.end()) {
        return (&animation->second);
    }

    return nullptr;
}

SpriteTextureSheet::~SpriteTextureSheet() {
    if(texture_ != Display_GetDefaultTexture()) {
        plDestroyTexture(texture_, false);
    }
}

class Sprite {
public:
    enum SpriteType {
        TYPE_DEFAULT,    // Depth-tested, scaled manually and oriented
    } type_{TYPE_DEFAULT};

    Sprite(SpriteType type, SpriteTextureSheet *sheet, PLVector3 position, PLColour colour, float scale);
    ~Sprite();

    float GetScale() { return scale_; }
    void SetScale(float scale);

    PLVector3 GetPosition() { return position_; }
    void SetPosition(const PLVector3 &position);

    PLVector3 GetAngles() { return angles_; }
    void SetAngles(const PLVector3 &angles);

    PLColour GetColour() { return colour_; }
    void SetColour(const PLColour &colour);

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

    SpriteTextureSheet *sheet_{nullptr};
    SpriteAnimation *current_animation_{nullptr};

    unsigned int current_frame_{0};
    double frame_delay_{0};
};

Sprite::Sprite(SpriteType type, SpriteTextureSheet *sheet, PLVector3 position, PLColour colour, float scale) :
    type_(type),
    position_(position),
    colour_(colour),
    scale_(scale),
    sheet_(sheet) {
}

Sprite::~Sprite() = default;

void Sprite::Tick() {
    if(current_animation_ == nullptr) {
        return;
    }

    if(frame_delay_ < g_state.sim_ticks + TICKS_PER_SECOND) {
        current_frame_++;
        if(current_frame_ > (current_animation_->h_frames + current_animation_->v_frames)) {
            current_frame_ = 0;
        }
    }
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

void Sprite::SetPosition(const PLVector3 &position) {
    position_ = position;
}

void Sprite::SetAngles(const PLVector3 &angles) {
    angles_ = angles;
}

void Sprite::SetColour(const PLColour &colour) {
    colour_ = colour;
}
