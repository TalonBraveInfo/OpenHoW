/* OpenHOW
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
#pragma once

#include <GL/glew.h>

typedef struct GLState {
    const char *renderer;
    const char *vendor;

    GLint version_major;
    GLint version_minor;

    unsigned int max_texture_units;

    // todo, allocate this on InitDisplay!
    char extensions[4096][4096];
    unsigned int num_extensions;
} GLState;

extern GLState gl_state;

/////////////////////////////////////////////////////////

#define BLEND_ADDITIVE  GL_SRC_ALPHA, GL_ONE
#define BLEND_DEFAULT   GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA

inline static void GLClearBoundTextures(void) {
    for(unsigned int i = 0; i < gl_state.max_texture_units; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);
}

inline static void GLClearBoundFramebuffers(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_RENDERBUFFER, 0);
}

/////////////////////////////////////////////////////////
// MESH

typedef struct Vertex {
    PLVector3   position;
    PLVector3   normal;
    PLVector2   uv[16];
    PLColour    colour;
} Vertex;

typedef struct Triangle {
    unsigned int indices[3];
} Triangle;

typedef struct Mesh {
    Vertex *vertices;
    unsigned int num_vertices;

    Triangle *triangles;
    unsigned int num_triangles;

    GLenum draw_mode;

    GLenum primitive;
    GLenum primitive_restore;

    unsigned int vbo_vertex_array;
} Mesh;

inline static Mesh *CreateMesh(GLenum primitive, GLenum draw_mode, unsigned int num_triangles,
                               unsigned int num_vertices) {
    assert(num_vertices > 0);

    Mesh *out = calloc(1, sizeof(Mesh));
    if(out == NULL) {
        print_error("failed to allocate mesh, aborting!\n");
    }
    memset(out, 0, sizeof(Mesh));

    if(draw_mode != GL_DYNAMIC_DRAW && draw_mode != GL_STATIC_DRAW) {
        print_error("invalid draw mode, %d, passed for mesh, aborting!\n", draw_mode);
    }

    out->draw_mode      = draw_mode;
    out->primitive      = out->primitive_restore = primitive;
    out->num_triangles  = num_triangles;
    out->num_vertices   = num_vertices;

    if(num_triangles > 0) {
        out->triangles = calloc(num_triangles, sizeof(Triangle));
        if(out->triangles == NULL) {
            print_error("failed to allocate %d triangles for mesh, aborting!\n", num_triangles);
        }
    }

    glGenBuffers(1, &out->vbo_vertex_array);

    return out;
}

inline static void DeleteMesh(Mesh *mesh) {
    if(mesh == NULL) {
        return;
    }

    if(mesh->triangles != NULL) {
        pork_free(mesh->triangles);
    }

    if(mesh->vertices != NULL) {
        pork_free(mesh->vertices);
    }

    pork_free(mesh);
}

inline static void BeginDraw(Mesh *mesh) {
    memset(mesh->triangles, 0, sizeof(Triangle) * mesh->num_triangles);
    memset(mesh->vertices, 0, sizeof(Vertex) * mesh->num_vertices);
}

inline static void MeshVertexPosition(Mesh *mesh, uint32_t index, PLVector3 position) {
    if(index > mesh->num_vertices) {
        print_error("index %d is out of range, %d, aborting!\n", index, mesh->num_vertices);
    }
}

inline static void EndDraw(Mesh *mesh) {
    // todo, upload to GPU
}

inline static void DrawMesh(const Mesh *mesh) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_vertex_array);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (const void *)offsetof(Vertex, position));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (const void *)offsetof(Vertex, colour));
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (const void *)offsetof(Vertex, normal));
}