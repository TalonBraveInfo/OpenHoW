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

#include <PL/platform_filesystem.h>
#include "../engine.h"
#include "../model.h"
#include "loaders.h"

/************************************************************/
/* Hir Skeleton Format */

HirHandle* Hir_LoadFile(const char* path) {
    size_t hir_size = plGetFileSize(path);
    if(hir_size == 0) {
        LogWarn("Unexpected Hir size in \"%s\", aborting (%s)!\n", path, plGetError());
        return nullptr;
    }

    FILE *file = fopen(path, "rb");
    if(file == nullptr) {
        LogWarn("Failed to load \"%s\", aborting!\n", path);
        return nullptr;
    }

    typedef struct __attribute__((packed)) HirBone {
        int32_t parent;
        int16_t coords[3];
        int8_t  unknown[10];
    } HirBone;

    auto num_bones = (unsigned int)(hir_size / sizeof(HirBone));
    HirBone bones[num_bones];
    unsigned int rnum_bones = fread(bones, sizeof(HirBone), num_bones, file);
    u_fclose(file)

    if(rnum_bones != num_bones) {
        LogWarn("Failed to read in all bones, %d/%d, aborting!\n", rnum_bones, num_bones);
        return nullptr;
    }

    /* for debugging */
    static const char* bone_names[static_cast<int>(SkeletonBone::MAX_BONES)]={
            "Pelvis",
            "Spine",
            "Head",
            "UpperArm.L", "LowerArm.L", "Hand.L",
            "UpperArm.R", "LowerArm.R", "Hand.R",
            "UpperLeg.L", "LowerLeg.L", "Foot.L",
            "UpperLeg.R", "LowerLeg.R", "Foot.R",
    };

    /* in the long term, we won't have this here, we'll probably extend the format
     * to include the names of each bone (.skeleton format?) */
    if(static_cast<SkeletonBone>(num_bones) >= SkeletonBone::MAX_BONES) {
        LogWarn("Invalid number of bones, %d/%d, aborting!\n", num_bones, SkeletonBone::MAX_BONES);
        return nullptr;
    }

    auto* handle = static_cast<HirHandle *>(u_alloc(1, sizeof(HirHandle), true));
    handle->bones = static_cast<PLModelBone *>(u_alloc(num_bones, sizeof(PLModelBone), true));
    for(unsigned int i = 0; i < num_bones; ++i) {
        handle->bones[i].position = PLVector3(bones[i].coords[0], bones[i].coords[1], bones[i].coords[2]);
        handle->bones[i].parent = bones[i].parent;
        strcpy(handle->bones[i].name, bone_names[i]);
    }
    return handle;
}

void Hir_DestroyHandle(HirHandle* handle) {
    if(handle == nullptr) {
        return;
    }

    u_free(handle->bones);
    u_free(handle);
}
