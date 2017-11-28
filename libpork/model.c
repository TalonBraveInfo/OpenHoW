/* OpenHOW
 * Copyright (C) 2017-2018 Mark E Sowden
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
#include <pork/model.h>

/*  VTX Format Specification    */

typedef struct VTXCoord {
    int16_t x;
    int16_t y;
    int16_t z;
    uint16_t bone_index;
} VTXCoord;

/*  NO2 Format Specification    */
/* The NO2 format is used by Hogs of War to store pre-calculated normals
 * for the mesh, as far as we've determined.
 */

typedef struct NO2Index { // todo
    // D2C7003F 7EE456BD 38D75CBF 00004041
    int32_t unknown0;
    int32_t unknown1;
    int32_t unknown2;
    int32_t unknown3;
} NO2Index;

/*  FAC Format Specification    */

typedef struct FACHeader {
    uint32_t padding[4];    // This is always blank
    uint32_t num_triangles;    // Number of FACBlocks
    uint32_t unknown3;
} FACHeader;

// ?   I0   I1  I2   N0  N1   N2  ?    TI  P    ?   ?    ?   ?    S   T
// 01000F00 0E001300 0F000E00 13003420 50000000 30203134 00002031 001F001F
// 01001000 0F001300 10000F00 13003420 50000000 30203331 00003432 001F001F
// 01001100 10001300 11001000 13003120 50000000 36203331 00003220 021F001F
// 01001200 11001300 12001100 1300390D 50000000 36203333 00003220 031F021F

typedef struct __attribute__((packed)) FACTriangle {
    uint16_t unknown1;

    uint16_t indices[3];    // Vertex indices
    uint16_t normal[3];     // Normals
    uint16_t unknown11;     // ??
    uint16_t texture_index; // Matches TIM listed in MTD package.

    uint16_t padding;

    int32_t unknown2;
    int32_t unknown3;

    int16_t texture_coords[2];
} FACTriangle;

typedef struct __attribute__((packed)) FACQuad {
    uint32_t unknown4;
    uint32_t unknown5;

    uint16_t indices[4];    // Vertex indices

    uint16_t normal[4];     // Normals

    uint16_t texture_index; // Matches TIM listed in MTD package.

    uint16_t padding;

    uint16_t unknown3;
    int32_t unknown6;
    int16_t unknown7;
} FACQuad;

/*  HIR Format Specification
    Used to store our piggy bones.
*/

// P        X   Y    Z   ?    ?   ?    ?   ?

// 00000000 0000EBFF 01000000 00000000 00000000
// 01000000 06004FFF 02000000 00000000 00000000
// 01000000 FDFF9BFF 58000000 00000000 00000000
// 03000000 03000000 6E000000 00000000 00000000
// 04000000 FDFF0000 6F000000 00000000 00000000
// 01000000 FEFF9BFF A9FF0000 00000000 00000000

typedef struct __attribute__((packed)) HIRBone {
    uint32_t parent;    // not 100% on this, links in with
                        // animations from the looks of it, urgh...
    int16_t coords[3];

    int8_t unknown[10];
} HIRBone;

/*  ANIM Format Specification   */
/* Don't actually have a name / extension
 * for this one, so we'll just dub it 'anim'
 * for now.
 */

/*
 * 00000000 8AFFFFFF 02000000 00000000 00000000 00000000 00000000 00000000
 * 8C8BD5BD EE4695BC B88630BE 0000803F 3DDD9D3C CD372CBD 4E1FA9BE 0000803F
 * 51E1B8BC A3E2303D 87D81F3C 0000803F 9FBCA43F F246B83E FDAFBB3D 0000803F
 * D11BA13B 509AFABE 8D244E3C 0000803F 2E9B8D3E FD10D23D D640B1BE 0000803F
 * F0045DBF 72A5843E 66EECBBE 0000803F 62CD4740 3C4AA73F 151A4840 0000803F
 * F5F14ABF 3BB1593E 0815BA3E 0000803F 5E4425BE 86B3F33D 5EED7FBD 0000803F
 * F30AC93A ADA5B137 C43162BC 0000803F 7ACA8F3E E3682C3E E724033E 0000803F
 * E92BA3BA BC220BBE DA57963E 0000803F FB0BC9BA DFE89DB7 CB0F49BC 0000803F
 * EF5F9C3D FD2C27BC 1A2BB5BD 0000803F
 */

//////////////////////////////////////////////////////////////////////////////

HIRBone bones[MAX_BONES];

Model models[MAX_MODELS];

// cache the pig data into memory, since we
// share it between all of them anyway :)
void init_model_cache(void) {
    memset(models, 0, sizeof(Model) * MAX_MODELS);

    FILE *file = fopen("./" PORK_BASE_DIR "/pig.hir", "rb");
    if(file == NULL) {
        print_error("Failed to load ./" PORK_BASE_DIR "/pig.hir\n");
    }

    memset(bones, 0, sizeof(HIRBone) * MAX_BONES);
    unsigned int num_bones = (unsigned int)fread(bones, sizeof(HIRBone), MAX_BONES, file);
    if(num_bones < MAX_BONES) {
        print_error("Unexpected number of bones, %d, less than 32!\n", num_bones);
    }
    fclose(file);
}

void debug_draw_skeleton(void) { // todo, pass cur pig and display
    static PLMesh *mesh = NULL;
    if(mesh == NULL) {
        mesh = plCreateMesh(PLMESH_LINES, PL_DRAW_IMMEDIATE, 0, MAX_BONES * 2);
    }

#if 0 // debugging crap
    model.skeleton_mesh = plCreateMesh(
            PLMESH_LINES,
            PL_DRAW_IMMEDIATE,
            0,
            model.num_bones * 2
    );
    for(unsigned int i = 0, vert = 0; i < model.num_bones; i++, vert += 2) {
#if 0
        PRINT("BONE %d\n", i);
        PRINT("    parent(%d)\n", model.bones[i].parent);
        PRINT("    coords(%d %d %d)\n",
              model.bones[i].coords[0],
              model.bones[i].coords[1],
              model.bones[i].coords[2]);
#endif
        // Start
        plSetMeshVertexPosition3f(model.skeleton_mesh, vert,
                                  model.bones[i].coords[0],
                                  model.bones[i].coords[1],
                                  model.bones[i].coords[2]);
        plSetMeshVertexColour(model.skeleton_mesh, vert, plCreateColour4b(PL_COLOUR_RED));

        // End
        plSetMeshVertexPosition3f(model.skeleton_mesh, vert + 1,
                                  model.bones[model.bones[i].parent].coords[0],
                                  model.bones[model.bones[i].parent].coords[1],
                                  model.bones[model.bones[i].parent].coords[2]);
        plSetMeshVertexColour(model.skeleton_mesh, vert + 1, plCreateColour4b(PL_COLOUR_GREEN));
    }
#endif
}

Model *load_model(const char *path) {
    if(plGetFileExtension(path) != '\0') {
        printf("load_model: extension passed for model, %s!\n", path);
        return NULL;
    }

    // load the vertices for the model
    char new_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(new_path, "%s.vtx", path);
    FILE *file = fopen(new_path, "rb");
    if(file == NULL) {
        printf("load_model: failed to find %s!\n", new_path);
        return NULL;
    }

    VTXCoord coords[MAX_VERTICES];
    unsigned int num_vertices = (unsigned int)fread(coords, sizeof(VTXCoord), 2048, file);
    if(num_vertices == 0) {
        printf("load_model: invalid number of vertices in %s!\n", new_path);
        goto TIDY_UP;
    }

    Model *model = (Model*)malloc(sizeof(Model)); // todo, add to retained list, so we can manage it laterrrrr...

TIDY_UP:
    fclose(file);

    return model;
}
