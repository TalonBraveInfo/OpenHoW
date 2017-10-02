/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include "model_private.h"

enum {
    MDL_FLAG_FLAT   = (1 << 0),
    MDL_FLAG_UNLIT  = (1 << 1),
};

#define MAX_MODEL_NAME      128
#define MAX_TEXTURE_NAME    64

#define MAX_INDICES_PER_FACE    6
#define MIN_INDICES_PER_FACE    3

/* There seem to be two seperate model formats
 * used within the game, those that are static
 * and then a completely different type of header
 * for those that aren't.
 *
 * The texture names don't used fixed sizes in either,
 * in the static model the length of these is declared
 * but it doesn't appear this is the case in the animated
 * model format; the animated model format also seems
 * to support multiple textures.
 *
 * The first byte in the model format, is a flag which appears to
 * control how the model will be displayed within the game.
 *
 * Within medkit.mdl
 *  OFFSET ?? is X coord of vertex 1
 *  OFFSET 66 is Y coord of vertex 1
 *  OFFSET 6A is Z coord of vertex 1
 *  OFFSET 6E is X coord of vertex 2
 *  OFFSET 72 is Y coord of vertex 2
 *
 *  3 bytes between each vertex coord
 *  12 bytes for each coord set?
 *
 *  First 32bit int after texture name
 *  appears to be number of vertices
 *
 *  ?? ?? X  ?? ?? ?? Y  ?? ?? ?? Z  ??
 *  7E 71 41 3F 4C 1E 0D BC 9F 3C A8 3F
 *
 *  ????X X  ????Y Y  ????Z Z
 *  7E71413F 4C1E0DBC 9F3CA83F
 */

typedef struct __attribute__((packed)) MDLVertex {
    uint8_t unknown0[2];
    int8_t x_;
    int8_t x;
    uint8_t unknown1[2];
    int8_t y_;
    int8_t y;
    uint8_t unknown2[2];
    int8_t z_;
    int8_t z;
} MDLVertex;

// 04:00:00:00:B4:BC:79:00:00:00:00:00:00:00:00:00:
// BC:BC:79:00:1C:00:1F:00:1B:00:19:00:57:D0:76:00:
// 66:42:1B:00:B0:21:78:00:70:E4:19:00:5C:35:6C:00:
// A1:D5:1C:00:9C:48:6A:00:36:DF:1E:00
// Quads are 60 bytes long; immediately follow vertices

// 03:00:00:00:14:BD:79:00:00:00:00:00:01:00:00:00:
// 1A:BD:79:00:05:00:01:00:06:00:E6:25:12:00:A1:DE:
// 22:00:92:CB:08:00:09:C3:17:00:B6:65:22:00:D8:E0:
// 09:00
// Triangles are 50 bytes long

typedef struct MDLFace {
    uint8_t num_indices;
    uint16_t indices[5];
} MDLFace;

#if 0
    uint8_t unknown0;  // typically seeing this as 4?
    // could be number of verts per face?
    uint8_t unknown000;
    uint16_t unknown00;

    uint8_t unknown1[16]; // completely no clue for now!

    uint16_t indices[4];

    uint8_t unknown2[32]; // no clue either!
#endif

/////////////////////////////////////////////////////////////////

PLModel *_plLoadRequiemModel(const char *path) {
    FILE *file = fopen(path, "rb");
    if(file == NULL) {
        _plReportError(PL_RESULT_FILEREAD, plGetResultString(PL_RESULT_FILEREAD));
        return NULL;
    }

    // attempt to figure out if it's valid or not... ho boy...

    _plDebugPrint("%s\n", path);

#define AbortLoad(...) _plDebugPrint(__VA_ARGS__); _plReportError(PL_RESULT_FILEREAD, __VA_ARGS__); fclose(file)

    // check which flags have been set for this particular mesh
    int flags = fgetc(file);
    if(flags & MDL_FLAG_FLAT) {} // flat
    if(flags & MDL_FLAG_UNLIT) {} // unlit
    if(!(flags & MDL_FLAG_FLAT) && !(flags & MDL_FLAG_UNLIT)) {} // shaded

    uint32_t texture_name_length = 0;
    if(fread(&texture_name_length, sizeof(uint32_t), 1, file) != 1) {
        AbortLoad("Invalid file length, failed to get texture name length!");
        return NULL;
    }

    if(texture_name_length > MAX_TEXTURE_NAME || texture_name_length == 0) {
        AbortLoad("Invalid texture name length, %d!", texture_name_length);
        return NULL;
    }

    char texture_name[texture_name_length];
    if(fread(texture_name, sizeof(char), texture_name_length, file) != texture_name_length) {
        AbortLoad("Invalid file length, failed to get texture name!");
        return NULL;
    }

    uint16_t num_vertices;
    if(fread(&num_vertices, sizeof(uint16_t), 1, file) != 1) {
        AbortLoad("Invalid file length, failed to get number of vertices!");
        return NULL;
    }

    // todo, figure out these two unknown bytes, quads? iirc (we did discuss this)
    fseek(file, 2, SEEK_CUR);

    uint32_t num_faces;
    if(fread(&num_faces, sizeof(uint32_t), 1, file) != 1) {
        AbortLoad("Invalid file length, failed to get number of faces!");
        return NULL;
    }

    if(num_faces == 0) {
        AbortLoad("Invalid number of faces, %d!", num_faces);
        return NULL;
    }

    MDLVertex vertices[num_vertices];
    if(fread(vertices, sizeof(MDLVertex), num_vertices, file) != num_vertices) {
        AbortLoad("Invalid file length, failed to load vertices!");
        return NULL;
    }

    unsigned int num_triangles = 0;
    MDLFace faces[num_faces];
    memset(faces, 0, sizeof(MDLFace) * num_faces);
    for(unsigned int i = 0; i < num_faces; ++i) {
        long pos = ftell(file);
        if(fread(&faces[i].num_indices, sizeof(uint32_t), 1, file) != 1) {
            AbortLoad("Invalid file length, failed to load number of indices! (offset: %ld)", ftell(file));
            return NULL;
        }

        if(faces[i].num_indices < 3 || faces[i].num_indices > MAX_INDICES_PER_FACE) {
            AbortLoad("Invalid number of indices, %d, required for face %d! (offset: %ld)",
                      faces[i].num_indices, i, ftell(file));
            return NULL;
        }

        num_triangles += faces[i].num_indices - 2;

        fseek(file, 16, SEEK_CUR); // todo, figure these out
        if(fread(faces[i].indices, sizeof(uint16_t), faces[i].num_indices, file) != faces[i].num_indices) {
            AbortLoad("Invalid file length, failed to load indices!");
            return NULL;
        }

        if(faces[i].num_indices == 6) {
            fseek(file, 54, SEEK_CUR); // skip over unknown bytes for now
        } else if(faces[i].num_indices == 5) {
            fseek(file, 42, SEEK_CUR); // skip over unknown bytes for now
        } else if(faces[i].num_indices == 4) {
            fseek(file, 32, SEEK_CUR); // skip over unknown bytes for now
        } else if(faces[i].num_indices == 3) {
            fseek(file, 24, SEEK_CUR); // skip over unknown bytes for now
        }

        long npos = ftell(file);
        _plDebugPrint(" Read %ld bytes for face %d (indices %d)\n", npos - pos, i, faces[i].num_indices);
    }

    fclose(file);

    _plDebugPrint("    texture_name_length: %d\n", texture_name_length);
    _plDebugPrint("    texture_name:        %s\n", texture_name);
    _plDebugPrint("    num_vertices:        %d\n", num_vertices);
    for(unsigned int i = 0; i < num_vertices; ++i) {
        _plDebugPrint("      vertex(%d) x(%d) y(%d) z(%d)\n", i, vertices[i].x, vertices[i].y, vertices[i].z);
    }
    _plDebugPrint("    num_faces:           %d\n", num_faces);
    for(unsigned int i = 0; i < num_faces; ++i) {
        _plDebugPrint("      face(%d) num_indices(%d)\n", i, faces[i].num_indices);
    }
    _plDebugPrint("    num_triangles:       %d\n", num_triangles);

#if 1
    PLMesh *mesh = plCreateMesh(PLMESH_POINTS, PL_DRAW_IMMEDIATE, num_triangles, num_vertices);
    if(mesh == NULL) {
        return NULL;
    }

    srand(num_vertices);
    for(unsigned int i = 0; i < num_vertices; ++i) {
#if 0
        plSetMeshVertexPosition3f(out, i,
                                  (vertices[i].x_ + vertices[i].x) / 10,
                                  (vertices[i].y_ + vertices[i].y) / 10,
                                  (vertices[i].z_ + vertices[i].z) / 10);
#else
        plSetMeshVertexPosition3f(mesh, i,
                                  vertices[i].x_ * vertices[i].x,
                                  vertices[i].y_ * vertices[i].y,
                                  vertices[i].z_ * vertices[i].z);
#endif
        plSetMeshVertexColour(mesh, i, plCreateColour4b(
                (uint8_t) (rand() % 255), (uint8_t) (rand() % 255), (uint8_t) (rand() % 255), 255)
        );
    }
#endif

    PLModel *model = malloc(sizeof(PLModel));
    if(model == NULL) {
        _plReportError(PL_RESULT_MEMORYALLOC, plGetResultString(PL_RESULT_MEMORYALLOC));
        return NULL;
    }

    memset(model, 0, sizeof(PLModel));
    model->num_meshes       = 1;
    model->num_vertices     = num_vertices;
    model->num_triangles    = num_triangles;
    model->meshes[0]        = mesh;

    _plGenerateModelNormals(model);
    _plGenerateModelAABB(model);

    return model;
}