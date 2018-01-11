/* OpenHoW
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
#include <PL/platform_filesystem.h>

#include "pork_engine.h"
#include "pork_model.h"

PLModel *LoadVTXModel(const char *path) {
    FILE *vtx_file = fopen(path, "rb");
    if(vtx_file == NULL) {
        Error("failed to load vtx \"%s\", aborting!\n", path);
    }

    // read in the vertices

    typedef struct VTXCoord {
        int16_t x;
        int16_t y;
        int16_t z;
        uint16_t bone_index;
    } VTXCoord;
    unsigned int num_vertices = (unsigned int)(plGetFileSize(path) / sizeof(VTXCoord));
    VTXCoord vertices[num_vertices];
    if(fread(vertices, sizeof(VTXCoord), num_vertices, vtx_file) != num_vertices) {
        Error("failed to read in all vertices from \"%s\", aborting!\n", path);
    }

#if 0
    print_debug("%d vertices\n", num_vertices);
    for(unsigned int i = 0; i < num_vertices; ++i) {
        print_debug("vtx %d %d %d (%d)\n", vertices[i].x, vertices[i].y, vertices[i].z, vertices[i].bone_index);
    }
#endif

    char ftx_path[PL_SYSTEM_MAX_PATH];
    strncpy(ftx_path, path, strlen(path) - 3);
    strcat(ftx_path, "ftx");
    FILE *ftx_file = fopen(path, "rb");
    if(ftx_file == NULL) {
        Error("failed to load ftx \"%s\", aborting!\n", path);
    }

    char no2_path[PL_SYSTEM_MAX_PATH];
    strncpy(no2_path, path, strlen(path) - 3);
    strcat(no2_path, "no2");
    FILE *no2_file = fopen(path, "rb");
    if(no2_file == NULL) {
        Error("failed to load no2 \"%s\", aborting!\n", path);
    }

    return NULL;
}

// todo, load hir from anywhere - open the potential to have
// multiple hirs for different models :)
Bone *LoadBones(const char *path) {
    return NULL;
}

Animation *LoadAnimations(const char *path) {
    return NULL;
}

////////////////////////////////////////////////////////////////

ModelCache g_model_cache;

//00: Hip
//01: Spine
//02: Head
//03: UpperArm.L
//04: LowerArm.L
//05: Hand.L
//06: UpperArm.R
//07: LowerArm.R
//08: Hand.R
//09: UpperLeg.L
//10: LowerLeg.L
//11: Foot.L
//12: UpperLeg.R
//13: LowerLeg.R
//14: Foot.R

// cache the pigs data into memory, since we
// share it between all of them anyway :)
void CacheModelData(void) {
    memset(&g_model_cache, 0, sizeof(ModelCache));

    LogInfo("caching pig.hir\n");

    char hir_path[PL_SYSTEM_MAX_PATH];
    snprintf(hir_path, sizeof(hir_path), "%s/chars/pig.hir", g_state.base_path);
    size_t hir_bytes = plGetFileSize(hir_path);
    if(hir_bytes == 0) {
        Error("unexpected \"pig.hir\" size, aborting!\n(perhaps try copying your data again?)");
    }

    FILE *file = fopen(hir_path, "rb");
    if(file == NULL) {
        Error("failed to load \"%s\"!\n", hir_path);
    }

    /* HIR Format Specification
     * Used to store our piggy bones.
     */
    typedef struct __attribute__((packed)) HIRBone {
        uint32_t parent;
        int16_t coords[3];
        int8_t unknown[10];
    } HIRBone;

    g_model_cache.num_bones = (unsigned int)(hir_bytes / sizeof(HIRBone));
    if(g_model_cache.num_bones > MAX_BONES) {
        Error("number of bones within \"%s\" exceeds %d limit!\n", hir_path, MAX_BONES);
    }

    HIRBone bones[g_model_cache.num_bones];
    if(fread(bones, sizeof(HIRBone), g_model_cache.num_bones, file) != g_model_cache.num_bones) {
        Error("failed to read in all bones, expected %d!\n", g_model_cache.num_bones);
    }
    fclose(file);

    // scary shit - assumes this matches up with indices in pig.hir, if not, boo hoo!
    const char *bone_names[]={
          "Hip",
          "Spine",
          "Head",
          "UpperArm.L",
          "LowerArm.L",
          "Hand.L",
          "UpperArm.R",
          "LowerArm.R",
          "Hand.R",
          "UpperLeg.L",
          "LowerLeg.L",
          "Foot.L",
          "UpperLeg.R",
          "LowerLeg.R",
          "Foot.R",
    };

    // copy each bone into our global bones list
    for(unsigned int i = 0; i < g_model_cache.num_bones; ++i) {
        g_model_cache.bones[i].parent = bones[i].parent;
        g_model_cache.bones[i].coords.x = bones[i].coords[0];
        g_model_cache.bones[i].coords.y = bones[i].coords[1];
        g_model_cache.bones[i].coords.z = bones[i].coords[2];
        strncpy(g_model_cache.bones[i].name, bone_names[i], sizeof(g_model_cache.bones[i].name));

        //plScaleVector3f(&g_model_cache.bones[i].coords, 4);
    }

    // animations

    LogInfo("caching mcap.mad\n");

    char mcap_path[PL_SYSTEM_MAX_PATH];
    snprintf(mcap_path, sizeof(mcap_path), "%s/chars/anims/mcap.mad", g_state.base_path);

    // check the number of bytes making up the mcap; we'll use this
    // to determine the length of animations later
    size_t mcap_bytes = plGetFileSize(mcap_path);
    if(mcap_bytes < 272) {
        Error("unexpected \"mcap.mad\" size, %d, aborting!\n", mcap_bytes);
    }

    file = fopen(mcap_path, "rb");
    if(file == NULL) {
        Error("failed to load \"%s\", aborting!\n", mcap_path);
    }

    /* todo, split this up, as the psx version deals with these as separate files */
    const char *animation_names[ANI_END]={
            "Run cycle (normal)",
            "Run cycle (wounded)",
            "Run cycle (more wounded)",
            "Walk cycle (backwards)",
            "Turning on Spot",
            "Swimming",
            "Swimming like Rick",
            "Getting into Vehicles",
            "Jumping - Start",
            "Jumping - Middle",
            "Jumping - End",
            "Scramble",
            "Getting out Handgun",
            "Getting out Rifle",
            "Getting out Machine gun",
            "Getting out Heavy weapon",
            "Getting out Punch",
            "Getting out Grenade",
            "Getting out Sword / Knife",
            "Using Grenade",
            "Using Punch",
            "Sword / Knife",
            "Bayonet",
            "Aiming Handgun",
            "Aiming Rifle",
            "Aiming Machine gun",
            "Aiming Heavy weapon",
            "Standing around cycle 1",
            "Standing around cycle 2",
            "Very Wounded",
            "Lord Flash-Heart Pose",
            "Looking around",
            "Looking gormless",
            "Cowering",
            "Brushoff 1",
            "Brushoff 2",
            "Brushoff 3",
            "Sneeze",
            "Flying through air/falling",
            "Bouncing on B-Hind",
            "Getting to feet",
            "Celebration #1",
            "Celebration #2",
            "Celebration #3",
            "Salute",
            "Look back",
            "Thinking",
            "Dying #1",
            "Dying #2",
            "Dying #3",
            "Drowning",
            "Idle Cold",
            "Idle Hot",
            "Lay Mine",
            "Heal",
            "Pick pocket",
            "Air strike",
            "Hari Kiri",
            "Parachuting",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            ""
    };

    // run through each animation and copy it
    // into our global animation list
    for(unsigned int i = 0; i < ANI_END; ++i) {
        // basically a MAD package, but without the
        // file name
        struct __attribute__((packed)) {
            uint32_t offset;
            uint32_t length;
        } index;
        if(fread(&index, sizeof(index), 1, file) != 1) {
            Error("failed to read index, aborting!\n");
        }

        // position we'll return to for the next index
        long position = ftell(file);

        /* MCAP Format Specification
         * Used to store our piggy animations.
         */
        typedef struct __attribute__((packed)) MCAPKeyframe {
            int16_t unused;

            struct __attribute__((packed)) {
                int8_t x;
                int8_t y;
                int8_t z;
            } transforms[10];

            struct __attribute__((packed)) {
                float x;
                float y;
                float z;
                float w;
            } rotations[15];
        } MCAPKeyframe;

        unsigned int num_keyframes = (unsigned int) (index.length / sizeof(MCAPKeyframe));
        if(num_keyframes == 0) {
            Error("odd keyframe at index, aborting!\n");
        }

        // copy everything into our global animations array

        g_model_cache.animations[i].frames = malloc(sizeof(Keyframe) * num_keyframes);
        if(g_model_cache.animations[i].frames == NULL) {
            Error("failed to allocate for %d keyframes, aborting!\n", num_keyframes);
        }

        // move to where the first keyframe is
        if(fseek(file, index.offset, SEEK_SET) != 0) {
            Error("failed to seek to offset %d in file, aborting!\n", index.offset);
        }

        for(unsigned int j = 0; j < num_keyframes; ++j) {
            // read in the keyframe data
            MCAPKeyframe frame;
            if(fread(&frame, sizeof(MCAPKeyframe), 1, file) != 1) {
                Error("failed to read animation keyframe, aborting!\n");
            }

            // copy transforms
            for(unsigned int k = 0; k < 10; ++k) {
                g_model_cache.animations[i].frames[j].transforms[k].x = frame.transforms[k].x;
                g_model_cache.animations[i].frames[j].transforms[k].y = frame.transforms[k].y;
                g_model_cache.animations[i].frames[j].transforms[k].z = frame.transforms[k].z;
            }

            // copy rotations
            for(unsigned int k = 0; k < 15; ++k) {
                g_model_cache.animations[i].frames[j].rotations[k].x = frame.rotations[k].x;
                g_model_cache.animations[i].frames[j].rotations[k].y = frame.rotations[k].y;
                g_model_cache.animations[i].frames[j].rotations[k].z = frame.rotations[k].z;
                g_model_cache.animations[i].frames[j].rotations[k].w = frame.rotations[k].w;
            }
        }

        g_model_cache.animations[i].num_frames = num_keyframes;
        g_model_cache.animations[i].name = animation_names[i];

        // return us from whence we came
        if(fseek(file, position, SEEK_SET) != 0) {
            Error("failed to seek back to original position %d in file, aborting!\n", position);
        }
    }
    fclose(file);

#if 0 // debug
    for(unsigned int i = 0; i < ANI_END; ++i) {
        for(unsigned int j = 0; j < g_model_cache.animations[i].num_frames; ++j) {
            print_debug("anim(%s) frame(%d) transformation(x:%d y:%d z:%d) angles(%d %d %d %d)\n",
                        g_model_cache.animations[i].name, j,

                        // print out the first transform
                        (int)g_model_cache.animations[i].frames[j].transforms[0].x,
                        (int)g_model_cache.animations[i].frames[j].transforms[0].y,
                        (int)g_model_cache.animations[i].frames[j].transforms[0].z,

                        // print out first rotation
                        (int)g_model_cache.animations[i].frames[j].rotations[0].x,
                        (int)g_model_cache.animations[i].frames[j].rotations[0].y,
                        (int)g_model_cache.animations[i].frames[j].rotations[0].z,
                        (int)g_model_cache.animations[i].frames[j].rotations[0].w
            );
        }
    }
#endif

    char pig_model_path[PL_SYSTEM_MAX_PATH];
    snprintf(pig_model_path, sizeof(pig_model_path), "%s/chars/british/ac_hi.vtx", g_state.base_path);
    g_model_cache.pigs[PIG_CLASS_ACE] = plLoadModel(pig_model_path);
    //g_model_cache.pigs[PIG_CLASS_COMMANDO] = plLoadModel(pig_model_path);
    //g_model_cache.pigs[PIG_CLASS_GRUNT] = plLoadModel(pig_model_path);
}

void InitModels(void) {
    plRegisterModelLoader("vtx", LoadVTXModel);

    CacheModelData();
}

////////////////////////////////////////////////////////////////

#if 1
#include <GL/glew.h>
#endif

void DEBUGDrawSkeleton(void) {
    if(!cv_debug_skeleton->b_value) {
        return;
    }

    static PLMesh *skeleton_mesh = NULL;
    if(skeleton_mesh == NULL) {
        skeleton_mesh = plCreateMesh(PL_MESH_LINES, PL_DRAW_IMMEDIATE, 0, g_model_cache.num_bones * 2);
    }

#if 1
    glPushMatrix();

    static float rotation = 90;
    rotation += 0.5f;
    glRotatef(0, 1, 0, 0);
    glRotatef(rotation, 0, 1, 0);
    glRotatef(180.f, 0, 0, 1);
#endif

    plClearMesh(skeleton_mesh);

    static unsigned int frame = 0;
    static double delay = 20;
    if(g_state.sim_ticks > delay) {
        frame++;
        delay = g_state.sim_ticks + 20;
    }

    if(frame == g_model_cache.animations[0].num_frames) {
        frame = 0;
    }

    for(unsigned int i = 0, vert = 0; i < g_model_cache.num_bones; ++i, vert += 2) {
        //start
        plSetMeshVertexPosition(skeleton_mesh, vert, g_model_cache.bones[i].coords);
        plSetMeshVertexColour(skeleton_mesh, vert, PLColour(255, 0, 0, 255));

        //end
        plSetMeshVertexPosition(skeleton_mesh, vert + 1, g_model_cache.bones[g_model_cache.bones[i].parent].coords);
        plSetMeshVertexColour(skeleton_mesh, vert + 1, PLColour(0, 255, 0, 255));
    }

    plUploadMesh(skeleton_mesh);
    plDrawMesh(skeleton_mesh);

#if 1
    glPopMatrix();
#endif
}