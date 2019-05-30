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
#include <PL/platform_mesh.h>
#include <PL/platform_model.h>

#include "engine.h"
#include "model.h"
#include "loaders/loaders.h"
#include "game/TempGame.h"

#include "graphics/display.h"
#include "graphics/shader.h"

/************************************************************/

struct {
    HirHandle*      pig_skeleton;

    Animation       animations[MAX_ANIMATIONS];
    unsigned int    num_animations;

    PLModel* pigs[MAX_CLASSES];
    PLModel* hats[MAX_CLASSES];
} model_cache;

static PLModel* default_model = NULL;   /* used if any model fails to load */

#define PIG_EYES_INDEX  118
#define PIG_GOBS_INDEX  119

static PLModel* Model_LoadVtxFile(const char* path) {
    VtxHandle* vtx = Vtx_LoadFile(path);
    if(vtx == NULL) {
        LogWarn("Failed to load Vtx, \"%s\"!\n", path);
        return NULL;
    }

    /* attempt to load the normals */
    char no2_path[PL_SYSTEM_MAX_PATH];
    strncpy(no2_path, path, strlen(path) - 3);
    no2_path[strlen(path) - 3] = '\0';
    strcat(no2_path, "no2");
    bool gen_normals = false;
    if(No2_LoadFile(no2_path, vtx) == NULL) {
        LogWarn("Failed to load normals, \"%s\"!\n", no2_path);
        /* in this case, we'll just generate them later */
        gen_normals = true;
    }

    /* now we load in all the faces */
    char fac_path[PL_SYSTEM_MAX_PATH];
    strncpy(fac_path, path, strlen(path) - 3);
    fac_path[strlen(path) - 3] = '\0';
    strcat(fac_path, "fac");
    FacHandle* fac = Fac_LoadFile(fac_path);
    if(fac == NULL) {
        Vtx_DestroyHandle(vtx);
        LogWarn("Failed to load Fac, \"%s\"!\n", path);
        return NULL;
    }

#if 0 /* dumped! */
    /* figure out how many textures we have for this model
     * so that we can generate a mesh for each later */

#define MAX_TEXTURE_INDICES 32
    unsigned int texture_indices[MAX_TEXTURE_INDICES];
    unsigned int num_texture_indices = 0;
    memset(texture_indices, -1, sizeof(unsigned int) * MAX_TEXTURE_INDICES);

    for(unsigned int i = 0; i < num_quads; ++i) {
        for(unsigned int j = 0; j < num_texture_indices; ++j) {
            if(quads[i].texture_index == texture_indices[j]) {
                goto NEXT_QUAD;
            }
        }

        u_assert(num_texture_indices < MAX_TEXTURE_INDICES);
        texture_indices[num_texture_indices++] = quads[i].texture_index;

        NEXT_QUAD:;
    }

    for(unsigned int i = 0; i < num_triangles; ++i) {
        for(unsigned int j = 0; j < num_texture_indices; ++j) {
            if(triangles[i].texture_index == texture_indices[j]) {
                goto NEXT_TRIANGLE;
            }
        }

        u_assert(num_texture_indices < MAX_TEXTURE_INDICES);
        texture_indices[num_texture_indices++] = triangles[i].texture_index;

        NEXT_TRIANGLE:;
    }

    /* now group together all our "meshes" */

    struct {
        struct {
            int8_t uv_a[2];
            int8_t uv_b[2];
            int8_t uv_c[2];

            uint16_t vertex_indices[3];
            uint16_t normal_indices[3];

            uint32_t texture_index;
        } triangles[4096]; /* temporary until I can be assed */
        unsigned int num_triangles;
    } meshes[num_texture_indices];

    for(unsigned int i = 0; i < num_texture_indices; ++i) {
        for(unsigned int j = 0; j < num_quads; ++j) {
            /* todo */
        }

        for(unsigned int j = 0; j < num_triangles; ++j) {
            if(texture_indices[i] != triangles[j].texture_index) {
                continue;
            }

            meshes[i].triangles[meshes[i].num_triangles].texture_index = triangles[j].texture_index;

            /* copy vertex coordinates into our group */
            for(unsigned int k = 0; k < 3; ++k) {
                meshes[i].triangles[meshes[i].num_triangles].vertex_indices[k] = triangles[j].vertex_indices[k];
                meshes[i].triangles[meshes[i].num_triangles].normal_indices[k] = triangles[j].normal_indices[k];
            }

            /* copy uv coordinates into our group */
            for(unsigned int k = 0; k < 2; ++k) {
                meshes[i].triangles[meshes[i].num_triangles].uv_a[k] = triangles[j].uv_a[k];
                meshes[i].triangles[meshes[i].num_triangles].uv_b[k] = triangles[j].uv_b[k];
                meshes[i].triangles[meshes[i].num_triangles].uv_c[k] = triangles[j].uv_c[k];
            }

            meshes[i].num_triangles++;
            u_assert(meshes[i].num_triangles < 4096);
        }
    }

    /* and allocate all of our meshes */

    PLMesh **model_meshes = u_alloc(1, sizeof(PLMesh*), true);
    for(unsigned int i = 0; i < num_texture_indices; ++i) {
        model_meshes[i] = plCreateMesh(PL_MESH_TRIANGLES, PL_DRAW_DYNAMIC, meshes[i].num_triangles, vtx->num_vertices);
        if(model_meshes[i] == NULL) {
            Error("Failed to allocate mesh for %s, aborting (%s)!\n", path, plGetError());
        }

        for(unsigned int j = 0; j < vtx->num_vertices; ++j) {
            plSetMeshVertexPosition(model_meshes[i], j, vtx->vertices[j].position);

#if 1 /* debug */
            uint8_t r = (uint8_t)(rand() % 255);
            uint8_t g = (uint8_t)(rand() % 255);
            uint8_t b = (uint8_t)(rand() % 255);
            plSetMeshVertexColour(model_meshes[i], j, PLColour(r, g, b, 255));
#else
            plSetMeshVertexColour(cur_mesh, j, PLColour(255, 255, 255, 255));
#endif

            model_meshes[i]->vertices[i].bone_index = vtx->vertices[i].bone_index;
            model_meshes[i]->vertices[i].bone_weight = 1.f;
        }

        unsigned int cur_index = 0;
        for(unsigned int j = 0; j < meshes[i].num_triangles; ++j) {
            plSetMeshTrianglePosition(model_meshes[i], &cur_index,
                                      meshes[i].triangles[j].vertex_indices[0],
                                      meshes[i].triangles[j].vertex_indices[1],
                                      meshes[i].triangles[j].vertex_indices[2]
            );
        }
    }
#endif

    PLMesh *mesh = plCreateMesh(PL_MESH_TRIANGLES, PL_DRAW_DYNAMIC, fac->num_triangles, vtx->num_vertices);
    if(mesh == NULL) {
        Vtx_DestroyHandle(vtx);
        Fac_DestroyHandle(fac);
        LogWarn("Failed to create mesh (%s)!\n", plGetError());
        return NULL;
    }

    for(unsigned int j = 0; j < vtx->num_vertices; ++j) {
        plSetMeshVertexPosition(mesh, j,
                PLVector3(
                        vtx->vertices[j].position.x * -1,
                        vtx->vertices[j].position.y * -1,
                        vtx->vertices[j].position.z * -1));
        plSetMeshVertexST(mesh, j, 0, 0);

#if 1 /* debug */
        uint8_t r = (uint8_t)(rand() % 255);
        uint8_t g = (uint8_t)(rand() % 255);
        uint8_t b = (uint8_t)(rand() % 255);
        plSetMeshVertexColour(mesh, j, PLColour(r, g, b, 255));
#else
        plSetMeshVertexColour(mesh, j, PLColour(255, 255, 255, 255));
#endif

        mesh->vertices[j].bone_index = vtx->vertices[j].bone_index;
        mesh->vertices[j].bone_weight = 1.f;
    }

    mesh->texture = Display_GetDefaultTexture();

    unsigned int cur_index = 0;
    for(unsigned int j = 0; j < fac->num_triangles; ++j) {
        plSetMeshTrianglePosition(mesh, &cur_index,
                                  fac->triangles[j].vertex_indices[0],
                                  fac->triangles[j].vertex_indices[1],
                                  fac->triangles[j].vertex_indices[2]
        );
    }

    if(gen_normals) {
        plGenerateMeshNormals(mesh);
    }

    PLModelBone* skeleton = u_alloc(model_cache.pig_skeleton->num_bones, sizeof(PLModelBone), true);
    memcpy(skeleton, model_cache.pig_skeleton->bones, sizeof(PLModelBone) * model_cache.pig_skeleton->num_bones);

    PLMesh** model_meshes = u_alloc(1, sizeof(PLMesh*), true);
    model_meshes[0] = mesh;
    PLModel *model = plNewSkeletalModel(
            &(PLModelLod){model_meshes, 1}, 1,
            skeleton, model_cache.pig_skeleton->num_bones, BONE_INDEX_PELVIS);
    if(model == NULL) {
        LogWarn("Failed to create model (%s)!\n", plGetError());
        return NULL;
    }

    plGenerateModelBounds(model);

#if 0 /* don't bother for now... */
    /* check if it's a LOD model, these are appended with '_hi' */
    char file_name[16];
    snprintf(file_name, sizeof(file_name), plGetFileName(path));
    if(file_name[0] != '\0' && strstr(file_name, "_hi") != 0) {
        char lod_path[PL_SYSTEM_MAX_PATH];
        strncpy(lod_path, path, strlen(path) - 7);  /* _hi.vtx */
        lod_path[strlen(path) - 7] = '\0';          /* _hi.vtx */
        strcat(lod_path, "_med.vtx");
        if(plFileExists(lod_path)) {
            LogDebug("found lod, \"%s\", adding to model\n", lod_path);
            model->num_lods += 1;
        } else {
            LogWarn("model name ends with \"_hi\" but no other LOD found with name \"%s\", ignoring!\n", lod_path);
        }
    }
#endif

    return model;
}

PLModel* Model_LoadMinFile(const char *path) {
    u_assert(0, "TODO");
    return NULL;
}

////////////////////////////////////////////////////////////////

/* like plLoadModel, only prefixes with base path
 * and can abort on error */
PLModel* Model_LoadFile(const char *path, bool abort_on_fail) {
    char model_path[PL_SYSTEM_MAX_PATH];
    snprintf(model_path, sizeof(model_path), "%s.vtx", u_find(path));
    if(!plFileExists(model_path)) {
        snprintf(model_path, sizeof(model_path), "%s.min", u_find(path));
    }

    PLModel* model = plLoadModel(model_path);
    if(model == NULL) {
        if(abort_on_fail) {
            Error("Failed to load model, \"%s\", aborting (%s)!\n", model_path, plGetError());
        }

        LogWarn("Failed to load model, \"%s\" (%s)!\n", model_path, plGetError());
        model = default_model;
    }
    return model;
}

void Model_DestroyHandle(PLModel* model) {
    /* destroy model */
    if(model == default_model) {
        return;
    }

    plDestroyModel(model);
}

Animation* LoadAnimations(const char *path, bool abort_on_fail) {
    return NULL;
}

/************************************************************/

/* cache the pigs data into memory, since we
 * share it between all of them anyway :) */
void CacheModelData(void) {
    memset(&model_cache, 0, sizeof(model_cache));

    model_cache.pig_skeleton = Hir_LoadFile(u_find("/chars/pig.hir"));
    if(model_cache.pig_skeleton == NULL) {
        Error("Failed to load skeleton, aborting!\n");
    }

    // animations

    LogInfo("Caching mcap.mad\n");

    char mcap_path[PL_SYSTEM_MAX_PATH];
    strncpy(mcap_path, u_find("/chars/mcap.mad"), sizeof(mcap_path));

    // check the number of bytes making up the mcap; we'll use this
    // to determine the length of animations later
    size_t mcap_bytes = plGetFileSize(mcap_path);
    if(mcap_bytes < 272) {
        Error("Unexpected \"mcap.mad\" size, %d, aborting!\n", mcap_bytes);
    }

    FILE* file = fopen(mcap_path, "rb");
    if(file == NULL) {
        Error("Failed to load \"%s\", aborting!\n", mcap_path);
    }

    /* todo, split this up, as the psx version deals with these as separate files */
    static const char* animation_names[] = {
            "Run cycle (normal)", "Run cycle (wounded)", "Run cycle (more wounded)", "Walk cycle (backwards)",
            "Turning on Spot",
            "Swimming", "Swimming like Rick",
            "Getting into Vehicles",
            "Jumping - Start", "Jumping - Middle", "Jumping - End",
            "Scramble",
            "Getting out Handgun", "Getting out Rifle", "Getting out Machine gun", "Getting out Heavy weapon",
            "Getting out Punch", "Getting out Grenade", "Getting out Sword / Knife",
            "Using Grenade", "Using Punch",
            "Sword / Knife",
            "Bayonet",
            "Aiming Handgun", "Aiming Rifle", "Aiming Machine gun", "Aiming Heavy weapon",
            "Standing around cycle 1", "Standing around cycle 2",
            "Very Wounded",
            "Lord Flash-Heart Pose",
            "Looking around", "Looking gormless",
            "Cowering",
            "Brushoff 1", "Brushoff 2", "Brushoff 3",
            "Sneeze",
            "Flying through air/falling",
            "Bouncing on B-Hind",
            "Getting to feet",
            "Celebration #1", "Celebration #2", "Celebration #3",
            "Salute",
            "Look back",
            "Thinking",
            "Dying #1", "Dying #2", "Dying #3",
            "Drowning",
            "Idle Cold", "Idle Hot",
            "Lay Mine",
            "Heal",
            "Pick pocket",
            "Air strike",
            "Hari Kiri",
            "Parachuting",
    };

    // run through each animation and copy it
    // into our global animation list
    for(unsigned int i = 0; i < MAX_ANIMATIONS; ++i) {
        // basically a MAD package, but without the
        // file name
        struct __attribute__((packed)) {
            uint32_t offset;
            uint32_t length;
        } index;
        if(fread(&index, sizeof(index), 1, file) != 1) {
            Error("Failed to read index, aborting!\n");
        }

        // position we'll return to for the next index
        long position = ftell(file);

        /* MCAP Format Specification
         * Used to store our piggy animations. */
        typedef struct __attribute__((packed)) McapKeyframe {
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
        } McapKeyframe;

        unsigned int num_keyframes = (unsigned int) (index.length / sizeof(McapKeyframe));
        if(num_keyframes == 0) {
            Error("Odd keyframe at index, aborting!\n");
        }

        // copy everything into our global animations array

        model_cache.animations[i].frames = u_alloc(num_keyframes, sizeof(Keyframe), true);

        // move to where the first keyframe is
        if(fseek(file, index.offset, SEEK_SET) != 0) {
            Error("Failed to seek to offset %d in file, aborting!\n", index.offset);
        }

        for(unsigned int j = 0; j < num_keyframes; ++j) {
            // read in the keyframe data
            McapKeyframe frame;
            if(fread(&frame, sizeof(McapKeyframe), 1, file) != 1) {
                Error("Failed to read animation keyframe, aborting!\n");
            }

            // copy transforms
            for(unsigned int k = 0; k < 10; ++k) {
                model_cache.animations[i].frames[j].transforms[k].x = frame.transforms[k].x;
                model_cache.animations[i].frames[j].transforms[k].y = frame.transforms[k].y;
                model_cache.animations[i].frames[j].transforms[k].z = frame.transforms[k].z;
            }

            // copy rotations
            for(unsigned int k = 0; k < 15; ++k) {
                model_cache.animations[i].frames[j].rotations[k].x = frame.rotations[k].x;
                model_cache.animations[i].frames[j].rotations[k].y = frame.rotations[k].y;
                model_cache.animations[i].frames[j].rotations[k].z = frame.rotations[k].z;
                model_cache.animations[i].frames[j].rotations[k].w = frame.rotations[k].w;
            }
        }

        model_cache.animations[i].num_frames = num_keyframes;
        if(i < plArrayElements(animation_names)) {
            model_cache.animations[i].name = animation_names[i];
        }

        // return us from whence we came
        if(fseek(file, position, SEEK_SET) != 0) {
            Error("Failed to seek back to original position %d in file, aborting!\n", position);
        }
    }
    u_fclose(file);

#if 0 // debug
    for(unsigned int i = 0; i < ANI_END; ++i) {
        for(unsigned int j = 0; j < g_model_cache.animations[i].num_frames; ++j) {
            print_debug("anim(%s) frame(%d) transformation(x:%d y:%d z:%d) angles(%d %d %d %d)\n",
                        g_model_cache.animations[i].name, j,

                        // print out the first transform
                        (int)model_cache.animations[i].frames[j].transforms[0].x,
                        (int)model_cache.animations[i].frames[j].transforms[0].y,
                        (int)model_cache.animations[i].frames[j].transforms[0].z,

                        // print out first rotation
                        (int)model_cache.animations[i].frames[j].rotations[0].x,
                        (int)model_cache.animations[i].frames[j].rotations[0].y,
                        (int)model_cache.animations[i].frames[j].rotations[0].z,
                        (int)model_cache.animations[i].frames[j].rotations[0].w
            );
        }
    }
#endif

    /* models */

    PLMesh* default_mesh = plCreateMesh(PL_MESH_LINES, PL_DRAW_DYNAMIC, 0, 6);
    plSetMeshVertexPosition(default_mesh, 0, PLVector3(0, 20, 0));
    plSetMeshVertexPosition(default_mesh, 1, PLVector3(0, -20, 0));
    plSetMeshVertexPosition(default_mesh, 2, PLVector3(20, 0, 0));
    plSetMeshVertexPosition(default_mesh, 3, PLVector3(-20, 0, 0));
    plSetMeshVertexPosition(default_mesh, 4, PLVector3(0, 0, 20));
    plSetMeshVertexPosition(default_mesh, 5, PLVector3(0, 0, -20));
    plSetMeshUniformColour(default_mesh, PLColour(255, 0, 0, 255));
    plSetMeshShaderProgram(default_mesh, programs[SHADER_UNTEXTURED]);
    plUploadMesh(default_mesh);
    default_model = plNewBasicStaticModel(default_mesh);

    model_cache.pigs[PIG_CLASS_ACE]         = Model_LoadFile("chars/pigs/ac_hi", true);
    model_cache.pigs[PIG_CLASS_COMMANDO]    = Model_LoadFile("chars/pigs/sb_hi", true);
    model_cache.pigs[PIG_CLASS_GRUNT]       = Model_LoadFile("chars/pigs/gr_hi", true);
    model_cache.pigs[PIG_CLASS_HEAVY]       = Model_LoadFile("chars/pigs/hv_hi", true);
    model_cache.pigs[PIG_CLASS_LEGEND]      = Model_LoadFile("chars/pigs/le_hi", true);
    model_cache.pigs[PIG_CLASS_MEDIC]       = Model_LoadFile("chars/pigs/me_hi", true);
    model_cache.pigs[PIG_CLASS_SABOTEUR]    = Model_LoadFile("chars/pigs/sa_hi", true);
    model_cache.pigs[PIG_CLASS_SNIPER]      = Model_LoadFile("chars/pigs/sn_hi", true);
    model_cache.pigs[PIG_CLASS_SPY]         = Model_LoadFile("chars/pigs/sp_hi", true);
}

void RegisterModelLoaders(void) {
    plRegisterModelLoader("vtx", Model_LoadVtxFile);
    plRegisterModelLoader("min", Model_LoadMinFile);
}

void ShutdownModels(void) {
    /* clear the model cache */
    LogInfo("Clearing model cache...\n");

    plDestroyModel(default_model);

    for(unsigned int i = 0; i < MAX_CLASSES; ++i) {
        plDestroyModel(model_cache.pigs[i]);
    }
}

////////////////////////////////////////////////////////////////

void DEBUGDrawModel(void) {
#if 1
    model_cache.pigs[PIG_CLASS_ACE]->model_matrix = plTranslateMatrix(PLVector3(0, 0, 512));
    plDrawModel(model_cache.pigs[PIG_CLASS_ACE]);

    model_cache.pigs[PIG_CLASS_COMMANDO]->model_matrix = plTranslateMatrix(PLVector3(0, 1024, 512));
    plDrawModel(model_cache.pigs[PIG_CLASS_COMMANDO]);
#else
    for(unsigned int y = 0; y < 80; y++) {
        for(unsigned int x = 0; x < 80; x++) {
            model_cache.pigs[PIG_CLASS_ACE]->model_matrix = plTranslateMatrix(PLVector3(x * 500, 0, y * 500));
            plDrawModel(model_cache.pigs[PIG_CLASS_ACE]);
            g_state.gfx.num_actors_drawn++;
        }
    }
#endif
}

void DEBUGDrawSkeleton(void) {
    if(!cv_debug_skeleton->b_value) {
        return;
    }

    static PLMesh* skeleton_mesh = NULL;
    if(skeleton_mesh == NULL) {
        skeleton_mesh = plCreateMesh(PL_MESH_LINES, PL_DRAW_DYNAMIC, 0, model_cache.pig_skeleton->num_bones * 2);
    }

#if 0
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
    if(g_state.sys_ticks > delay) {
        frame++;
        delay = g_state.sys_ticks + 20;
    }

    if(frame == model_cache.animations[0].num_frames) {
        frame = 0;
    }

    for(unsigned int i = 0, vert = 0; i < model_cache.pig_skeleton->num_bones; ++i, vert += 2) {
        PLModelBone* bone = &model_cache.pig_skeleton->bones[i];

        //start
        plSetMeshVertexPosition(skeleton_mesh, vert, bone->position);
        plSetMeshVertexColour(skeleton_mesh, vert, PLColour(255, 0, 0, 255));

        //end
        plSetMeshVertexPosition(skeleton_mesh, vert + 1, model_cache.pig_skeleton->bones[bone->parent].position);
        plSetMeshVertexColour(skeleton_mesh, vert + 1, PLColour(0, 255, 0, 255));
    }

    plSetNamedShaderUniformMatrix4x4(NULL, "pl_model", plMatrix4x4Identity(), false);
    plUploadMesh(skeleton_mesh);
    plDrawMesh(skeleton_mesh);

#if 0
    glPopMatrix();
#endif
}