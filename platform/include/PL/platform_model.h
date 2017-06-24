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

#pragma once

#include "platform.h"
#include "platform_math.h"
#include "platform_graphics.h"

enum {
    PL_MODELTYPE_STATIC,
    PL_MODELTYPE_ANIMATED,
    PL_MODELTYPE_SKELETAL
};

typedef struct PLModelFrame {
    PLTriangle  *triangles;
    PLVertex    *vertices;

    PLVector3D mins, maxs; // Bounds
} PLModelFrame;

// Static animated mesh.
typedef struct PLStaticModel {
    PLuint num_triangles;
    PLuint num_vertices;

    PLMeshPrimitive primitive;

    PLModelFrame frame;
} PLStaticModel;

// Per-vertex animated mesh.
typedef struct PLAnimatedModel {
    PLuint num_triangles;
    PLuint num_vertices;
    PLuint num_frames;

    PLMeshPrimitive primitive;

    PLModelFrame *frames;
} PLAnimatedModel;

// Mesh with bone structure.
typedef struct PLSkeletalModel {
    PLuint num_triangles;
    PLuint num_vertices;

    PLMeshPrimitive primitive;

    // Unfinished...
} PLSkeletalModel;

#include "platform_model_u3d.h"
#include "platform_model_obj.h"

PL_EXTERN_C

// Static
PLStaticModel *plCreateStaticModel(void);
PLStaticModel *plLoadStaticModel(const PLchar *path);
void plDeleteStaticModel(PLStaticModel *model);

// Animated
PLAnimatedModel *plCreateAnimatedModel(void);
PLAnimatedModel *plLoadAnimatedModel(const PLchar *path);
void plDeleteAnimatedModel(PLAnimatedModel *model);

PLAnimatedModel *plLoadU3DModel(const PLchar *path);

// Utility
void plGenerateStaticModelNormals(PLStaticModel *model);
void plGenerateAnimatedModelNormals(PLAnimatedModel *model);
void plGenerateSkeletalModelNormals(PLSkeletalModel *model);

PLVector3D plGenerateVertexNormal(PLVector3D a, PLVector3D b, PLVector3D c);

PL_EXTERN_C_END
