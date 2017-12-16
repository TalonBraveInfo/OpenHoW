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

    PLTexture *texture;
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

BitmapFont *LoadBitmapFont(const char *path, const char *tab_path) {
    FILE *tab_file = fopen(tab_path, "rb");
    if(tab_file == NULL) {
        print_error("failed to load tab \"%s\", aborting!\n", tab_path);
    }

    // seek over some crap
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

    // todo, load in the image

    return NULL;
}

//////////////////////////////////////////////////////////////////////////

void InitFonts(void) {
#if 0
    g_fonts[FONT_BIG] = ;
    g_fonts[FONT_BIG_CHARS] = ;
    g_fonts[FONT_CHARS2] = ;
    g_fonts[FONT_CHARS3] = ;
    g_fonts[FONT_GAME_CHARS] = ;
    g_fonts[FONT_SMALL] = ;
#endif
}

void ShutdownFonts(void) {

}