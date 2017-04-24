
#pragma once

typedef enum PLPrimitive {
    PL_PRIMITIVE_IGNORE,

    PL_PRIMITIVE_LINES,
    PL_PRIMITIVE_LINE_STRIP,
    PL_PRIMITIVE_POINTS,
    PL_PRIMITIVE_TRIANGLES,
    PL_PRIMITIVE_TRIANGLE_STRIP,
    PL_PRIMITIVE_TRIANGLE_FAN,
    PL_PRIMITIVE_TRIANGLE_FAN_LINE,
    PL_PRIMITIVE_QUADS,

    PL_NUM_PRIMITIVES
} PLPrimitive;

typedef enum PLDrawMode {
    PL_DRAW_DYNAMIC,
    PL_DRAW_STATIC,
    PL_DRAW_IMMEDIATE,  // Not necessarily supported in all cases, will just revert to dynamic otherwise!

    PL_NUM_DRAWMODES
} PLDrawMode;

typedef struct PLVertex {
    PLVector3D position, normal;
    PLVector2D st[16];

    PLColour colour;
} PLVertex;

typedef struct PLTriangle {
    PLVector3D normal;

    PLuint indices[3];
} PLTriangle;

#if defined(PL_MODE_OPENGL)
/*  Draw calls are horribly complicated in modern GL
 *  and require us to track a number of different indexes
 *  all at once, rather than just the one.
 *
 *  Because of this and for the sake of time, this is why
 *  there is this horrid collection below and 'mode'-specific
 *  code here which I would rather live without in the libraries
 *  own headers.
 *
 *  I can't really think of a solution to this at the moment
 *  because generally the structs here should be the same
 *  regardless of which rendering API we've chosen (otherwise
 *  people using the library might have to refactor code or make
 *  changes depending on which rendering API they want to use which
 *  makes this all slightly redundant).
 *
 *  One solution might be to have a seperate dynamically allocated
 *  array within the platform library itself which carries all of these
 *  for us and knows which ones are assigned to which mesh objects...
 *  But that sounds so overly complicated and stupid that the better
 *  solution in the end might be to just leave this how it is.
 *
 *  ~hogsy
 */
enum {
    _PL_MESH_VERTICES,
    _PL_MESH_INDICES,

    _PL_NUM_MESHINDEXES
};
#endif

typedef struct PLMesh {
#if defined(PL_MODE_OPENGL)
    PLuint id[_PL_NUM_MESHINDEXES];
#else
    PLuint id;
#endif

    PLVertex *vertices;
    PLTriangle *triangles;
    PLuint8 *indices;

    PLuint numverts;
    PLuint numtriangles;

    PLPrimitive primitive, primitive_restore;
    PLDrawMode mode;

    PLVector3D position, angles;
} PLMesh;

PL_EXTERN_C

PL_EXTERN PLMesh *plCreateMesh(PLPrimitive primitive, PLDrawMode mode, PLuint num_tris, PLuint num_verts);
PL_EXTERN PLMesh *plCreateRectangleMesh(PLint x, PLint y, PLuint width, PLuint height);
PL_EXTERN PLMesh *plCreateTriangleMesh(PLint x, PLint y, PLuint width, PLuint height);
PL_EXTERN void plDeleteMesh(PLMesh *mesh);

PL_EXTERN void plClearMesh(PLMesh *mesh);
PL_EXTERN void plSetMeshVertexPosition(PLMesh *mesh, PLuint index, PLVector3D vector);
PL_EXTERN void plSetMeshVertexPosition3f(PLMesh *mesh, PLuint index, PLfloat x, PLfloat y, PLfloat z);
PL_EXTERN void plSetMeshVertexPosition2f(PLMesh *mesh, PLuint index, PLfloat x, PLfloat y);
PL_EXTERN void plSetMeshVertexNormal3f(PLMesh *mesh, unsigned int index, float x, float y, float z);
PL_EXTERN void plSetMeshVertexST(PLMesh *mesh, PLuint index, PLfloat s, PLfloat t);
PL_EXTERN void plSetMeshVertexColour(PLMesh *mesh, PLuint index, PLColour colour);
PL_EXTERN void plUploadMesh(PLMesh *mesh);

PL_EXTERN void plDrawMesh(PLMesh *mesh);

PL_EXTERN_C_END
