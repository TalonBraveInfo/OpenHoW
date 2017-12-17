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
#include "engine_gl.h"

#include <PL/platform_filesystem.h>

typedef struct BitmapChar {
    unsigned int x;
    unsigned int y;

    unsigned int w;
    unsigned int h;

    char character;

    float s;
    float t;
} BitmapChar;

typedef struct BitmapFont {
    BitmapChar chars[128];
    unsigned int num_chars;

    unsigned int width;
    unsigned int height;

    GLuint texture_id;
} BitmapFont;

enum {
    FONT_BIG,
    FONT_BIG_CHARS,
    FONT_CHARS2,
    FONT_CHARS3,
    FONT_GAME_CHARS,
    FONT_SMALL,

    NUM_FONTS
};

BitmapFont *g_fonts[NUM_FONTS];

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

    // seek over some crap? (what was this again... ???)
    fseek(tab_file, 16, SEEK_SET);

    struct {
        // C0035A00 02001E00

        uint8_t a0; uint8_t a1; // ?
        uint8_t b0; uint8_t b1; // ?

        uint16_t width;
        uint16_t height;
    } tab_indices[128];
    unsigned int num_chars = (unsigned int)fread(tab_indices, sizeof(tab_indices) / 128, 128, tab_file);
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

    for(unsigned int i = 0; i < font->num_chars; ++i) {
        font->chars[i].character = (unsigned char)(33 + i);
        font->chars[i].w = tab_indices[i].width;
        font->chars[i].h = tab_indices[i].height;

        if(i < 1) {
            continue;
        }

        font->chars[i].x = font->chars[i - 1].w + font->chars[i - 1].x;
        font->chars[i].y = font->chars[i - 1].y;
        if(font->chars[i].x > font->width) {
            font->chars[i].y += font->chars[i].h;
        }
    }
#if 0
    PLTexture *texture = plCreateTexture();
    if(texture == NULL) {
        print_error("failed to create texture, %s, aborting!\n", plGetError());
    }

    plUploadTextureImage(texture, &image);
#endif
    plFreeImage(&image);

    return font;
}

//////////////////////////////////////////////////////////////////////////

void InitFonts(void) {
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
            break;
        }

        glDeleteTextures(1, &g_fonts[i]->texture_id);

        free(g_fonts[i]);
    }
}