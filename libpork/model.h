/* OpenHOW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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

typedef struct Bone {
    unsigned int parent;
    PLVector3 coords;
} Bone;

typedef struct Keyframe {
    PLVector3 transforms[10];
    PLQuaternion rotations[15];
} Keyframe;

typedef struct Animation {
    unsigned int id;
    const char *name;

    Keyframe *frames;
    unsigned int num_frames;
} Animation;

typedef struct ModelCache {
    Bone bones[MAX_BONES];
    Animation animations[ANI_END];
} ModelCache;

extern ModelCache g_model_cache;

void CacheModelData(void);