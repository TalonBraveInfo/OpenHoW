/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2011-2016 OldTimes Software

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "PL/platform.h"

#include "PL/platform_model.h"

/*
MD2 Model Format

Model format introduced in id Software's Quake 2.
*/

#define MD2_HEADER          (('2'<<24)+('P'<<16)+('D'<<8)+'I')
#define MD2_FILE_EXTENSION  ".md2"
#define MD2_VERSION         8

#define MD2_MAX_FRAMES      1024
#define MD2_MAX_SKINS       32
#define MD2_MAX_TRIANGLES   4096
#define MD2_MAX_VERTICES    8192

typedef struct {
    int16_t index_xyz[3];
    int16_t index_st[3];
} MD2Triangle_t;

typedef struct {
    unsigned char v[3];                // scaled byte to fit in frame mins/maxs
    unsigned char lightnormalindex;
} MD2TriangleVertex_t;

typedef struct {
    float scale[3];                    // multiply byte verts by this
    float translate[3];                // then add this

    PLchar name[16];                // frame name from grabbing
    MD2TriangleVertex_t verts[1];    // variable sized
} MD2Frame_t;

typedef struct {
    int16_t S, T;
} MD2TextureCoordinate_t;

typedef struct {
    int ident;
    int version;
    unsigned int skinwidth;
    unsigned int skinheight;
    int framesize;        // Byte size of each frame.
    int num_skins;
    int num_xyz;
    int num_st;            // Greater than num_xyz for seams.
    int numtris;
    int num_glcmds;        // Dwords in strip/fan command list.
    int num_frames;
    int ofs_skins;        // Each skin is a MAX_SKINNAME string.
    int ofs_st;            // Byte offset from start for stverts.
    int ofs_tris;        // Offset for dtriangles.
    int ofs_frames;        // Offset for first frame.
    int ofs_glcmds;
    int ofs_end;        // End of file.
} MD2_t;

PLAnimatedModel *plLoadMD2Model(const PLchar *path) {
    if (!path || path[0] == ' ')
        return nullptr;

    return nullptr;
}
