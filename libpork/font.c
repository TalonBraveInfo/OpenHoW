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
#include "engine.h"
#include "font.h"

#include <PL/platform_filesystem.h>

BitmapFont *g_fonts[NUM_FONTS];

PLMesh *font_mesh = NULL;

void DrawBitmapCharacter(BitmapFont *font, int x, int y, float scale, uint8_t character) {
    // ensure that the character we're being passed fits within HoW's bitmap set
    if(character < 33 || character > 138) {
        return;
    }
    character -= 31;

    if(font->texture == NULL) {
        print_error("attempted to draw bitmap font with invalid texture, aborting!\n");
    }

    if(font_mesh == NULL) {
        print_error("attempted to draw font before font init, aborting!\n");
    }

    plClearMesh(font_mesh);
    font_mesh->texture = font->texture;

    plSetMeshUniformColour(font_mesh, PLColour(255, 255, 255, 255));

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

void DrawBitmapString(BitmapFont *font, int x, int y, float scale, const char *msg) {
    unsigned int num_chars = (unsigned int)strlen(msg);
    if(num_chars == 0) {
        return;
    }

    if(font->texture == NULL) {
        print_error("attempted to draw bitmap font with invalid texture, aborting!\n");
    }

    plSetBlendMode(PL_BLEND_ADDITIVE);

    for(unsigned int i = 0; i < num_chars; ++i) {
        DrawBitmapCharacter(font, x, y, scale, (uint8_t) msg[i]);
        if(msg[i] >= 33 && msg[i] <= 122) {
            x += font->chars[msg[i] - 31].w;
        } else {
            x += 5;
        }

    }

    plSetBlendMode(PL_BLEND_DISABLE);
}

BitmapFont *LoadBitmapFont(const char *path) {
    char tab_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    snprintf(tab_path, sizeof(tab_path), "%s.tab", path);
    if(!plFileExists(tab_path)) {
        print_error("failed to load tab \"%s\", aborting!\n", tab_path);
    }

    // todo, we don't care about the image type...
    char tex_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    snprintf(tex_path, sizeof(tex_path), "%s.bmp", path);
    if(!plFileExists(tex_path)) {
        print_error("failed to load tab \"%s\", aborting!\n", tex_path);
    }

    FILE *tab_file = fopen(tab_path, "rb");
    if(tab_file == NULL) {
        print_error("failed to load tab \"%s\", aborting!\n", tab_path);
    }

#define MAX_CHARS   128
    struct {
        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
    } tab_indices[MAX_CHARS];
    unsigned int num_chars = (unsigned int)fread(tab_indices, sizeof(tab_indices) / MAX_CHARS, MAX_CHARS, tab_file);
    if(num_chars == 0) {
        print_error("invalid number of characters for \"%s\", aborting!\n", tab_path);
    }
    fclose(tab_file);

    // todo, load in the image

    PLImage image;
    if(plLoadImage(tex_path, &image) != PL_RESULT_SUCCESS) {
        print_error("failed to load in image, %s, aborting!\n", plGetError());
    }

    BitmapFont *font = calloc(1, sizeof(BitmapFont));
    memset(font, 0, sizeof(BitmapFont));

    font->width = image.width;
    font->height = image.height;
    font->num_chars = num_chars;

    unsigned int origin_x = tab_indices[2].x;
    unsigned int origin_y = tab_indices[2].y;
    for(unsigned int i = 0; i < font->num_chars; ++i) {
        font->chars[i].w = tab_indices[i].w;
        font->chars[i].h = tab_indices[i].h;
        font->chars[i].x = tab_indices[i].x - origin_x;
        font->chars[i].y = tab_indices[i].y - origin_y;
#if 1 // debug
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
        print_error("failed to create texture for font, %s, aborting!\n", plGetError());
    }

    font->texture->filter = PL_TEXTURE_FILTER_NEAREST;

    plUploadTextureImage(font->texture, &image);
    plFreeImage(&image);

    return font;
}

//////////////////////////////////////////////////////////////////////////

void InitFonts(void) {
    font_mesh = plCreateMesh(PL_TRIANGLE_STRIP, PL_DRAW_IMMEDIATE, 2, 4);
    if(font_mesh == NULL) {
        print_error("failed to create font mesh, %s, aborting!\n", plGetError());
    }

    g_fonts[FONT_BIG]        = LoadBitmapFont("./" PORK_FONTS_DIR "/big");
    g_fonts[FONT_BIG_CHARS]  = LoadBitmapFont("./" PORK_FONTS_DIR "/bigchars");
    g_fonts[FONT_CHARS2]     = LoadBitmapFont("./" PORK_FONTS_DIR "/chars2");
    g_fonts[FONT_CHARS3]     = LoadBitmapFont("./" PORK_FONTS_DIR "/chars3");
    g_fonts[FONT_GAME_CHARS] = LoadBitmapFont("./" PORK_FONTS_DIR "/gamechars");
    g_fonts[FONT_SMALL]      = LoadBitmapFont("./" PORK_FONTS_DIR "/small");
}

void ShutdownFonts(void) {
    for(unsigned int i = 0; i < NUM_FONTS; ++i) {
        if(g_fonts[i] == NULL) {
            /* if we hit a null slot, it's possible the fonts
             * failed loading at this point. so we'll just
             * break here.
             */
            print_debug("hit null font in shutdown fonts, skipping the rest!\n");
            break;
        }

        plDeleteTexture(g_fonts[i]->texture, true);
        free(g_fonts[i]);
    }
}