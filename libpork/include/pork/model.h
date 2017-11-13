/*  OpenHOW Model Format Specification  */

#pragma once

#include <pork/pork.h>

//////////////////////////////////////////////////////////////////////////////////

#define MAX_MODELS  4096    // originally I was thinking to size this dynamically, but is there even a need?

#define MAX_VERTICES    2048
#define MAX_TRIANGLES   4096
#define MAX_QUADS       4096
#define MAX_BONES       32

typedef struct Model {

    FACTriangle triangles[MAX_TRIANGLES];
    FACQuad     quads[MAX_QUADS];

    unsigned int num_vertices;
    unsigned int num_triangles; // triangles * (quads * 2)
    unsigned int num_bones;
    unsigned int num_meshes;

    PLMesh *tri_mesh;       // Our actual output!
    PLMesh *skeleton_mesh;  // preview of skeleton

    PLMesh **meshes;
} Model;

void init_model_cache(void);