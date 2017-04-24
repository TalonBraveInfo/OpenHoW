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

#include "platform_graphics.h"

#if defined(PL_MODE_OPENGL)
#   define _PL_USE_VERTEX_BUFFER_OBJECTS
#endif

typedef struct PLTranslatePrimitive {
    PLPrimitive mode;

    PLuint target;

    const PLchar *name;
} PLTranslatePrimitive;

PLTranslatePrimitive _pl_primitives[] = {
#if defined (PL_MODE_OPENGL)
        {PL_PRIMITIVE_LINES, GL_LINES, "LINES"},
        {PL_PRIMITIVE_POINTS, GL_POINTS, "POINTS"},
        {PL_PRIMITIVE_TRIANGLES, GL_TRIANGLES, "TRIANGLES"},
        {PL_PRIMITIVE_TRIANGLE_FAN, GL_TRIANGLE_FAN, "TRIANGLE_FAN"},
        {PL_PRIMITIVE_TRIANGLE_FAN_LINE, GL_LINES, "TRIANGLE_FAN_LINE"},
        {PL_PRIMITIVE_TRIANGLE_STRIP, GL_TRIANGLE_STRIP, "TRIANGLE_STRIP"},
        {PL_PRIMITIVE_QUADS, GL_QUADS, "QUADS"}
#elif defined (VL_MODE_GLIDE)
{ PL_PRIMITIVE_LINES,					GR_LINES,			"LINES" },
    { PL_PRIMITIVE_LINE_STRIP,				GR_LINE_STRIP,		"LINE_STRIP" },
    { PL_PRIMITIVE_POINTS,					GR_POINTS,			"POINTS" },
    { PL_PRIMITIVE_TRIANGLES,				GR_TRIANGLES,		"TRIANGLES" },
    { PL_PRIMITIVE_TRIANGLE_FAN,			GR_TRIANGLE_FAN,	"TRIANGLE_FAN" },
    { PL_PRIMITIVE_TRIANGLE_FAN_LINE,		GR_LINES,			"TRIANGLE_FAN_LINE" },
    { PL_PRIMITIVE_TRIANGLE_STRIP,			GR_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
    { PL_PRIMITIVE_QUADS,					0,					"QUADS" }
#elif defined (VL_MODE_DIRECT3D)
#elif defined (VL_MODE_VULKAN)
    { PL_PRIMITIVE_LINES,					VK_PRIMITIVE_TOPOLOGY_LINE_LIST,		"LINES" },
    { PL_PRIMITIVE_POINTS,					VK_PRIMITIVE_TOPOLOGY_POINT_LIST,		"POINTS" },
    { PL_PRIMITIVE_TRIANGLES,				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,	"TRIANGLES" },
    { PL_PRIMITIVE_TRIANGLE_FAN,			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,		"TRIANGLE_FAN" },
    { PL_PRIMITIVE_TRIANGLE_FAN_LINE,		VK_PRIMITIVE_TOPOLOGY_LINE_LIST,		"TRIANGLE_FAN_LINE" },
    { PL_PRIMITIVE_TRIANGLE_STRIP,			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
    { PL_PRIMITIVE_QUADS,					0,										"QUADS" }
#else
    { 0 }
#endif
};

PLuint _plTranslatePrimitiveMode(PLPrimitive mode) {
    if(mode == PL_PRIMITIVE_IGNORE) {
        return 0;
    }

    for (PLint i = 0; i < plArrayElements(_pl_primitives); i++) {
        if (mode == _pl_primitives[i].mode)
            return _pl_primitives[i].target;
    }

    // Hacky, but just return initial otherwise.
    return _pl_primitives[0].target;
}

PLuint _plTranslateDrawMode(PLDrawMode mode) {
#if defined(PL_MODE_OPENGL)
    if(mode == PL_DRAW_DYNAMIC) {
        return GL_DYNAMIC_DRAW;
    } else if(mode == PL_DRAW_STATIC) {
        return GL_STATIC_DRAW;
    }

    return 0;
#else
    return mode;
#endif
}

void _plApplyMeshLighting(PLMesh *mesh, PLLight *light, PLVector3D position) {
    PLVector3D distvec = position;
    plSubtractVector3D(&distvec, light->position);
    PLfloat distance = (light->radius - plVector3DLength(distvec)) / 100.f;

    for(PLuint i = 0; i < mesh->numverts; i++) {
    }

#if 0
    // Calculate the distance.
    PLVector3f distvec = { 0 };
    plVectorSubtract3fv(position, light->position, distvec);
    float distance = (light->radius - plLengthf(distvec)) / 100.0f;

    for(PLuint i = 0; i < object->numverts; i++)
    {
        float x = object->vertices[i].normal[0];
        float y = object->vertices[i].normal[1];
        float z = object->vertices[i].normal[2];

        float angle = (distance * ((x * distvec[0]) + (y * distvec[1]) + (z * distvec[2])));
        if(angle < 0)
            object->vertices[i].colour.Clear();
        else
        {
            object->vertices[i].colour[PL_RED]      = light->colour[PL_RED] * angle;
            object->vertices[i].colour[PL_GREEN]    = light->colour[PL_GREEN] * angle;
            object->vertices[i].colour[PL_BLUE]     = light->colour[PL_BLUE] * angle;
        }

        /*
        x = Object->Vertices_normalStat[count].x;
        y = Object->Vertices_normalStat[count].y;
        z = Object->Vertices_normalStat[count].z;

        angle = (LightDist*((x * Object->Spotlight.x) + (y * Object->Spotlight.y) + (z * Object->Spotlight.z) ));
        if (angle<0 )
        {
        Object->Vertices_screen[count].r = 0;
        Object->Vertices_screen[count].b = 0;
        Object->Vertices_screen[count].g = 0;
        }
        else
        {
        Object->Vertices_screen[count].r = Object->Vertices_local[count].r * angle;
        Object->Vertices_screen[count].b = Object->Vertices_local[count].b * angle;
        Object->Vertices_screen[count].g = Object->Vertices_local[count].g * angle;
        }
        */
    }
#endif
}

void _plDrawArrays(PLPrimitive mode, PLuint first, PLuint count) {
    if ((count == 0) || (first > count)) {
        return;
    }

#if defined(PL_MODE_OPENGL)
    glDrawArrays(_plTranslatePrimitiveMode(mode), first, count);
#endif
}

void _plDrawElements(PLPrimitive mode, PLuint count, PLuint type, const PLvoid *indices) {
    if ((count == 0) || !indices) {
        plSetError("Invalid number of indices when drawing object!\n");
        return;
    }

#if defined(PL_MODE_OPENGL)
    glDrawElements(_plTranslatePrimitiveMode(mode), count, type, indices);
#endif
}

PLMesh *plCreateMesh(PLPrimitive primitive, PLDrawMode mode, PLuint num_tris, PLuint num_verts) {
    plFunctionStart();

    if(primitive == PL_PRIMITIVE_IGNORE) {
        plSetError("Primitive cannot be set to 'ignore' upon creation!\n");
        return NULL;
    } else if((primitive < PL_PRIMITIVE_IGNORE) || (primitive > PL_NUM_PRIMITIVES)) {
        plSetError("Invalid primitive! (%d)\n", primitive);
        return NULL;
    }

    PLuint umode = _plTranslateDrawMode(mode);
    if(!umode && (mode != PL_DRAW_IMMEDIATE)) {
        plSetError("Invalid/unsupported draw mode! (%d)\n", mode);
        return NULL;
    }

    PLMesh *mesh = (PLMesh*)calloc(1, sizeof(PLMesh));
    if(!mesh) {
        plSetError("Failed to create mesh!\n");
        return NULL;
    }

    mesh->primitive = mesh->primitive_restore = primitive;
    mesh->numtriangles = num_tris;
    mesh->numverts = num_verts;
    mesh->mode = mode;

    if(num_tris > 0) {
        mesh->triangles = (PLTriangle*)calloc(num_tris, sizeof(PLTriangle));
        if(!mesh->triangles) {
            plSetError("Failed to allocate triangles for mesh! (%i)\n", num_tris);

            plDeleteMesh(mesh);
            return NULL;
        }
    }
    mesh->vertices = (PLVertex*)calloc(num_verts, sizeof(PLVertex));
    if(!mesh->vertices) {
        plSetError("Failed to allocate vertices for mesh! (%i)\n", num_verts);

        plDeleteMesh(mesh);
        return NULL;
    }

#if defined(PL_MODE_OPENGL) && defined(_PL_USE_VERTEX_BUFFER_OBJECTS)
    if(mode != PL_DRAW_IMMEDIATE) {
        glGenBuffers(1, &mesh->id[_PL_MESH_VERTICES]);
    }
#endif

    return mesh;
}

void plDeleteMesh(PLMesh *mesh) {
    if(!mesh) {
        return;
    }

    if(mesh->indices) {
        free(mesh->indices);
    }
    if(mesh->vertices) {
        free(mesh->vertices);
    }
    if(mesh->triangles) {
        free(mesh->triangles);
    }
    free(mesh);
}

void plClearMesh(PLMesh *mesh) {
    plFunctionStart();

    // Reset the data contained by the mesh, if we're going to begin a new draw.
    memset(mesh->vertices, 0, sizeof(PLVertex) * mesh->numverts);
    memset(mesh->triangles, 0, sizeof(PLTriangle) * mesh->numtriangles);
}

void plSetMeshVertexPosition(PLMesh *mesh, PLuint index, PLVector3D vector) {
    mesh->vertices[index].position = vector;
}

void plSetMeshVertexPosition3f(PLMesh *mesh, PLuint index, PLfloat x, PLfloat y, PLfloat z) {
    mesh->vertices[index].position = plCreateVector3D(x, y, z);
}

void plSetMeshVertexPosition2f(PLMesh *mesh, PLuint index, PLfloat x, PLfloat y) {
    mesh->vertices[index].position = plCreateVector3D(x, y, 0);
}

void plSetMeshVertexPosition3fv(PLMesh *mesh, PLuint index, PLuint size, const PLfloat *v) {
    plFunctionStart();

    size += index;
    if(size > mesh->numverts) {
        size -= (size - mesh->numverts);
    }

    for(PLuint i = index; i < size; i++) {
        mesh->vertices[i].position.x = v[0];
        mesh->vertices[i].position.y = v[1];
        mesh->vertices[i].position.z = v[2];
    }
}

void plSetMeshVertexNormal3f(PLMesh *mesh, unsigned int index, float x, float y, float z) {
    mesh->vertices[index].normal = plCreateVector3D(x, y, z);
}

void plSetMeshVertexST(PLMesh *mesh, PLuint index, PLfloat s, PLfloat t) {
    plFunctionStart();

    mesh->vertices[index].st[0] = plCreateVector2D(s, t);
}

void plSetMeshVertexSTv(PLMesh *mesh, PLbyte unit, PLuint index, PLuint size, const PLfloat *st) {
    plFunctionStart();

    size += index;
    if(size > mesh->numverts) {
        size -= (size - mesh->numverts);
    }

    for(PLuint i = index; i < size; i++) {
        mesh->vertices[i].st[unit].x = st[0];
        mesh->vertices[i].st[unit].y = st[1];
    }
}

void plSetMeshVertexColour(PLMesh *mesh, PLuint index, PLColour colour) {
    plFunctionStart();

    mesh->vertices[index].colour = colour;
}

void plUploadMesh(PLMesh *mesh) {
    plFunctionStart();

#if defined(PL_MODE_OPENGL) && defined(_PL_USE_VERTEX_BUFFER_OBJECTS)
    if((mesh->mode == PL_DRAW_IMMEDIATE) || (mesh->primitive == PL_PRIMITIVE_QUADS)) {
        // todo, eventually just convert quad primitives to a triangle strip or something...
        return;
    }

#if defined(PL_MODE_OPENGL_CORE)
    // Fill our buffer with data.
    glBindBuffer(GL_ARRAY_BUFFER, mesh->id[_PL_MESH_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(PLVertex), &mesh->vertices[0].position.x, _plTranslateDrawMode(mesh->mode));
#endif
#endif
}

void plDrawMesh(PLMesh *mesh) {
    if(!mesh->numverts) {
        return;
    }

#if defined(PL_MODE_OPENGL)
#if !defined(PL_MODE_OPENGL_CORE) //&& !defined(_PL_USE_VERTEX_BUFFER_OBJECTS)
    if(mesh->mode == PL_DRAW_IMMEDIATE) {
#if 1
        glBegin(_plTranslatePrimitiveMode(mesh->primitive));
        for(PLuint i = 0; i < mesh->numverts; i++) {
            glVertex3f(mesh->vertices[i].position.x, mesh->vertices[i].position.y, mesh->vertices[i].position.z);
            glTexCoord2f(mesh->vertices[i].st[0].x, mesh->vertices[i].st[0].y);
            glColor3ub(mesh->vertices[i].colour.r, mesh->vertices[i].colour.g, mesh->vertices[i].colour.b);
            glNormal3f(mesh->vertices[i].normal.x, mesh->vertices[i].normal.y, mesh->vertices[i].normal.z);
        }
        glEnd();
#else
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

        PLVertex *vert = &mesh->vertices[0];
        glVertexPointer(3, GL_FLOAT, 0, &vert->position);
        glColorPointer(4, GL_FLOAT, 0, &vert->colour);
        glNormalPointer(GL_FLOAT, 0, &vert->normal);
        for(PLint i = 0; i < plGetMaxTextureUnits(); i++) {
            if (pl_graphics_state.tmu[i].active) {
                glClientActiveTexture((GLenum) GL_TEXTURE0 + i);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2, GL_FLOAT, 0, vert->st);
            }
        }

        if(mesh->primitive == PL_PRIMITIVE_TRIANGLES) {
            _plDrawElements
                    (
                            mesh->primitive,
                            mesh->numtriangles * 3,
                            GL_UNSIGNED_BYTE,
                            mesh->indices
                    );
        } else {
            _plDrawArrays(mesh->primitive, 0, mesh->numverts);
        }

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        for(PLint i = 0; i < plGetMaxTextureUnits(); i++) {
            if(pl_graphics_state.tmu[i].active) {
                glClientActiveTexture((GLenum)GL_TEXTURE0 + i);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        }
#endif
    } else if(mesh->primitive != PL_PRIMITIVE_QUADS) {
#else
    {
#endif
        glBindBuffer(GL_ARRAY_BUFFER, mesh->id[_PL_MESH_VERTICES]);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        if (mesh->primitive == PL_PRIMITIVE_TRIANGLES) {
            _plDrawElements(
                    mesh->primitive,
                    mesh->numtriangles * 3,
                    GL_UNSIGNED_BYTE,
                    mesh->indices
            );
        } else {
            _plDrawArrays(mesh->primitive, 0, mesh->numverts);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
#endif
}

void plDrawMeshNormals(PLMesh *mesh) {
    if (mesh->primitive == PL_PRIMITIVE_LINES) {
        return;
    }

#if 0
    PLVector3D endpos;
    for (PLuint i = 0; i < mesh->numverts; i++) {
        endpos = (mesh->vertices[i].normal * 2.0f) + mesh->vertices[i].position;
        //plDrawLine blah
    }
#endif
}

/*
void plDraw(PLDraw *draw) {
    _PL_GRAPHICS_TRACK();
#if 0 // update this :(


#if defined(VL_MODE_GLIDE)
    // todo, glide needs its own setup here...
#elif defined(PL_MODE_OPENGL)
#if 0
    if(draw->_gl_vbo[_VL_BUFFER_VERTICES] != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_VERTICES]);
        glVertexPointer(3, GL_FLOAT, sizeof(PLVertex), NULL);

        glBindBuffer(GL_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_TEXCOORDS]);
        glTexCoordPointer(2, GL_FLOAT, sizeof(PLVertex), NULL);

        // todo, switch to using glInterleavedArrays?


    }
    else
#endif

#endif
#endif
}
 */

// Utility Functions

void plCreateCubeMesh() {}

PLMesh *plCreateRectangleMesh(PLint x, PLint y, PLuint width, PLuint height) {
    PLMesh *mesh = plCreateMesh(
            PL_PRIMITIVE_TRIANGLE_STRIP,
            PL_DRAW_IMMEDIATE, // todo, use dynamic!!
            2, 4
    );
    if(!mesh) {
        return NULL;
    }

    plClearMesh(mesh);
    plSetMeshVertexPosition2f(mesh, 0, x, y + height);
    plSetMeshVertexColour(mesh, 0, plCreateColour4b(255, 0, 0, 255));
    plSetMeshVertexPosition2f(mesh, 1, x, y);
    plSetMeshVertexPosition2f(mesh, 2, x + width, y + height);
    plSetMeshVertexPosition2f(mesh, 3, x + width, y);
    plUploadMesh(mesh);

    return mesh;
}

PLMesh *plCreateTriangleMesh(PLint x, PLint y, PLuint width, PLuint height) {
    PLMesh *mesh = plCreateMesh(
            PL_PRIMITIVE_TRIANGLE_FAN,
            PL_DRAW_IMMEDIATE, // todo, use dynamic!!
            1, 3
    );
    if(!mesh) {
        return NULL;
    }

    plClearMesh(mesh);
    plSetMeshVertexPosition2f(mesh, 0, x, y + height);
    plSetMeshVertexColour(mesh, 0, plCreateColour4b(255, 0, 0, 255));
    plSetMeshVertexPosition2f(mesh, 1, x + width / 2, x);
    plSetMeshVertexColour(mesh, 1, plCreateColour4b(0, 255, 0, 255));
    plSetMeshVertexPosition2f(mesh, 2, x + width, y + height);
    plSetMeshVertexColour(mesh, 2, plCreateColour4b(0, 0, 255, 255));
    plUploadMesh(mesh);

    return mesh;
}
