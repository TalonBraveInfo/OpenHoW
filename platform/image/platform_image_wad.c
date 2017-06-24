/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2017 Mark Elsworth Sowden <markelswo@gmail.com>

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

#include "PL/platform_image.h"

typedef struct WADHeader {
    char ident[4];    // WAD2, 2DAW, IWAD, PWAD

    int32_t num_lumps;
    int32_t table_offset;
} WADHeader;

typedef struct WADLump {
    int32_t file_pos;
    int32_t disk_size;
    int32_t size;

    char type;
    char compression;
    char pad1, pad2;
    char name[16];
} WADLump;

enum WADType {
    WAD_TYPE_NONE,
    WAD_TYPE_LABEL,

    WAD_TYPE_LUMPY = 64,
    WAD_TYPE_PALETTE = 64,
    WAD_TYPE_TEXTURE,
    WAD_TYPE_PIC,
    WAD_TYPE_SOUND,
    WAD_TYPE_MIPTEX
} WADType;

PLbool _plWADFormatCheck(FILE *fin) {
    rewind(fin);

    PLchar ident[4];
    fread(ident, sizeof(PLchar), 4, fin);

    return (PLbool)(strncmp(ident, "IWAD", 3) == 0);
}