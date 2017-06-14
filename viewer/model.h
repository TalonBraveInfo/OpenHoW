
#pragma once

#include "main.h"

#define MAX_VERTICES    2048
#define MAX_TRIANGLES   4096
#define MAX_QUADS       4096
#define MAX_BONES       32

typedef struct Model {
    unsigned int num_vertices;
    unsigned int num_triangles;
    unsigned int num_bones;
    unsigned int num_meshes;

    PLMesh **meshes;
} Model;