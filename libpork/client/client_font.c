/* OpenHoW
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
#include "pork_engine.h"
#include "client_font.h"

#include <PL/platform_filesystem.h>

BitmapFont *g_fonts[NUM_FONTS];

PLMesh *font_mesh = NULL;

void DrawBitmapCharacter(BitmapFont *font, int x, int y, float scale, PLColour colour, uint8_t character) {
    // ensure that the character we're being passed fits within HoW's bitmap set
    if(character < 33 || character > 138) {
        return;
    }
    character -= 33;

    if(font->texture == NULL) {
        Error("attempted to draw bitmap font with invalid texture, aborting!\n");
    }

    if(font_mesh == NULL) {
        Error("attempted to draw font before font init, aborting!\n");
    }

    plSetTexture(font->texture, 0);

    plClearMesh(font_mesh);

    plSetMeshUniformColour(font_mesh, colour);

    BitmapChar *bitmap_char = &font->chars[character];
    plSetMeshVertexPosition(font_mesh, 0, PLVector3(x, y, 0));
    plSetMeshVertexPosition(font_mesh, 1, PLVector3(x, y + bitmap_char->h, 0));
    plSetMeshVertexPosition(font_mesh, 2, PLVector3(x + bitmap_char->w, y, 0));
    plSetMeshVertexPosition(font_mesh, 3, PLVector3(x + bitmap_char->w, y + bitmap_char->h, 0));

    float tw = (float)bitmap_char->w / font->width;
    float th = (float)bitmap_char->h / font->height;
    float tx = (float)bitmap_char->x / font->width;
    float ty = (float)bitmap_char->y / font->height;
    plSetMeshVertexST(font_mesh, 0, tx, ty);
    plSetMeshVertexST(font_mesh, 1, tx, ty + th);
    plSetMeshVertexST(font_mesh, 2, tx + tw, ty);
    plSetMeshVertexST(font_mesh, 3, tx + tw, ty + th);

    plUploadMesh(font_mesh);
    plDrawMesh(font_mesh);
}

void DrawBitmapString(BitmapFont *font, int x, int y, unsigned int spacing, float scale, PLColour colour,
                      const char *msg) {
    unsigned int num_chars = (unsigned int)strlen(msg);
    if(num_chars == 0) {
        return;
    }

    if(font->texture == NULL) {
        Error("attempted to draw bitmap font with invalid texture, aborting!\n");
    }

    plSetBlendMode(PL_BLEND_ADDITIVE);

    int n_x = x;
    int n_y = y;
    for(unsigned int i = 0; i < num_chars; ++i) {
        DrawBitmapCharacter(font, n_x, n_y, scale, colour, (uint8_t) msg[i]);
        if(msg[i] >= 33 && msg[i] <= 122) {
            n_x += font->chars[msg[i] - 33].w + spacing;
        } else if(msg[i] == '\n') {
            n_y += font->chars[0].h;
            n_x = x;
        } else {
            n_x += 5;
        }
    }

    plSetBlendMode(PL_BLEND_DISABLE);
}

BitmapFont *LoadBitmapFont(const char *name, const char *tab_name) {
    char tab_path[PL_SYSTEM_MAX_PATH];
    snprintf(tab_path, sizeof(tab_path), "%sfe/text/%s.tab", GetBasePath(), tab_name);
    if(!plFileExists(tab_path)) {
        Error("failed to load tab for \"%s\", aborting!\n", name);
    }

    char tex_path[PL_SYSTEM_MAX_PATH];
    snprintf(tex_path, sizeof(tex_path), "%sfe/text/%s.bmp", GetBasePath(), name);
    if(!plFileExists(tex_path)) {
        /* try again, just in case it's in the TIM format */
        snprintf(tex_path, sizeof(tex_path), "%sfe/text/%s.tim", GetBasePath(), name);
        if(!plFileExists(tex_path)) {
            Error("failed to load texture for \"%s\", aborting!\n", name);
        }
    }

    FILE *tab_file = fopen(tab_path, "rb");
    if(tab_file == NULL) {
        Error("failed to load tab \"%s\", aborting!\n", tab_path);
    }

    fseek(tab_file, 16, SEEK_CUR);

#define MAX_CHARS   256
    struct {
        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
    } tab_indices[MAX_CHARS];
    unsigned int num_chars = (unsigned int)fread(tab_indices, sizeof(tab_indices) / MAX_CHARS, MAX_CHARS, tab_file);
    if(num_chars == 0) {
        Error("invalid number of characters for \"%s\", aborting!\n", tab_path);
    }
    fclose(tab_file);

    // todo, load in the image

    PLImage image;
    if(!plLoadImage(tex_path, &image)) {
        Error("failed to load in image, %s, aborting!\n", plGetError());
    }

    plReplaceImageColour(&image, PLColour(255, 0, 255, 255), PLColour(0, 0, 0, 0));

    BitmapFont *font = pork_alloc(1, sizeof(BitmapFont), true);
    memset(font, 0, sizeof(BitmapFont));

    font->width = image.width;
    font->height = image.height;
    font->num_chars = num_chars;

    unsigned int origin_x = tab_indices[0].x;
    unsigned int origin_y = tab_indices[0].y;
    for(unsigned int i = 0; i < font->num_chars; ++i) {
        font->chars[i].w = tab_indices[i].w;
        font->chars[i].h = tab_indices[i].h;
        font->chars[i].x = tab_indices[i].x - origin_x;
        font->chars[i].y = tab_indices[i].y - origin_y;
#if 0 // debug
        print(
                "font char %d: w(%d) h(%d) x(%d) y(%d)\n",
                i,
                font->chars[i].w,
                font->chars[i].h,
                font->chars[i].x,
                font->chars[i].y
        );
#endif
    }

    // upload the texture to the GPU

    font->texture = plCreateTexture();
    if(font->texture == NULL) {
        Error("failed to create texture for font, %s, aborting!\n", plGetError());
    }

    font->texture->filter = PL_TEXTURE_FILTER_NEAREST;

    plUploadTextureImage(font->texture, &image);
    plFreeImage(&image);

    return font;
}

//////////////////////////////////////////////////////////////////////////

void InitFonts(void) {
    font_mesh = plCreateMesh(PL_MESH_TRIANGLE_STRIP, PL_DRAW_IMMEDIATE, 2, 4);
    if(font_mesh == NULL) {
        Error("failed to create font mesh, %s, aborting!\n", plGetError());
    }

    g_fonts[FONT_BIG]        = LoadBitmapFont("big", "big");
    g_fonts[FONT_BIG_CHARS]  = LoadBitmapFont("bigchars", "bigchars");
    g_fonts[FONT_CHARS2]     = LoadBitmapFont("chars2l", "chars2");
    g_fonts[FONT_CHARS3]     = LoadBitmapFont("chars3", "chars3");
    g_fonts[FONT_GAME_CHARS] = LoadBitmapFont("gamechars", "gamechars");
    g_fonts[FONT_SMALL]      = LoadBitmapFont("small", "small");
}

void ShutdownFonts(void) {
    for(unsigned int i = 0; i < NUM_FONTS; ++i) {
        if(g_fonts[i] == NULL) {
            /* if we hit a null slot, it's possible the fonts
             * failed loading at this point. so we'll just
             * break here. */
            LogDebug("hit null font in shutdown fonts, skipping the rest!\n");
            break;
        }

        plDeleteTexture(g_fonts[i]->texture, true);
        free(g_fonts[i]);
    }
}