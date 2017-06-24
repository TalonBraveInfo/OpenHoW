
#pragma once

typedef enum PLPrimitive {
    PL_PRIMITIVE_IGNORE,

    PLMESH_LINES,
    PL_PRIMITIVE_LINE_STRIP,
    PLMESH_POINTS,
    PLMESH_TRIANGLES,
    PLMESH_TRIANGLE_STRIP,
    PLMESH_TRIANGLE_FAN,
    PLMESH_TRIANGLE_FAN_LINE,
    PLMESH_QUADS,

    PL_NUM_PRIMITIVES
} PLMeshPrimitive;

typedef enum PLDrawMode {
    PL_DRAW_DYNAMIC,
    PL_DRAW_STATIC,
    PL_DRAW_IMMEDIATE,  // Not necessarily supported in all cases, will just revert to dynamic otherwise!

    PL_NUM_DRAWMODES
} PLMeshDrawMode;

typedef struct PLVertex {
    PLVector3D position, normal;
    PLVector2D st[16];

    PLColour colour;
} PLVertex;

typedef struct PLTriangle {
    PLVector3D normal;

    unsigned int indices[3];
} PLTriangle;

#if defined(PL_MODE_OPENGL)
enum {
    _PLGL_BUFFER_VERTICES,
    _PLGL_BUFFER_TEXCOORDS,

    _PLGL_BUFFERS
};
#endif

typedef struct PLMesh {
#if defined(PL_MODE_OPENGL)
    unsigned int _buffers[_PLGL_BUFFERS];
#endif

    PLVertex *vertices;
    PLTriangle *triangles;
    uint8_t *indices;

    unsigned int num_verts;
    unsigned int num_triangles;

    PLMeshPrimitive primitive, primitive_restore;
    PLMeshDrawMode mode;

    PLVector3D position, angles;
} PLMesh;

PL_EXTERN_C

PL_EXTERN PLMesh *plCreateMesh(PLMeshPrimitive primitive, PLMeshDrawMode mode, PLuint num_tris, PLuint num_verts);
PL_EXTERN void plDeleteMesh(PLMesh *mesh);

PL_EXTERN void plDrawRectangle(PLRectangle rect);
PL_EXTERN void plDrawTriangle(int x, int y, unsigned int w, unsigned int h);

PL_EXTERN void plClearMesh(PLMesh *mesh);
PL_EXTERN void plSetMeshVertexPosition(PLMesh *mesh, unsigned int index, PLVector3D vector);
PL_EXTERN void plSetMeshVertexPosition3f(PLMesh *mesh, unsigned int index, float x, float y, float z);
PL_EXTERN void plSetMeshVertexPosition2f(PLMesh *mesh, unsigned int index, float x, float y);
PL_EXTERN void plSetMeshVertexNormal3f(PLMesh *mesh, unsigned int index, float x, float y, float z);
PL_EXTERN void plSetMeshVertexST(PLMesh *mesh, unsigned int index, float s, float t);
PL_EXTERN void plSetMeshVertexColour(PLMesh *mesh, unsigned int index, PLColour colour);
PL_EXTERN void plUploadMesh(PLMesh *mesh);

PL_EXTERN void plDrawMesh(PLMesh *mesh);

PL_EXTERN_C_END
