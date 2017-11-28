/* OpenHOW
 * Copyright (C) 2017-2018 Mark E Sowden
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

//#define DEBUG_FONTS

typedef struct PIGChar {
    int x, y;
    unsigned int width, height;
    char character;

#if defined(DEBUG_FONTS)
    GLuint texture;
#endif

    float s, t; // position within the bitmap
} PIGChar;

#define FONT_CHARACTERS 128

typedef struct PIGFont {
    PIGChar chars[FONT_CHARACTERS];
    unsigned int num_chars;

    unsigned int width, height;

    GLuint texture;
} PIGFont;

enum {
    FONT_BIG,
    FONT_BIG_CHARS,
    FONT_CHARS2,
    FONT_CHARS3,
    FONT_GAME_CHARS,
    FONT_SMALL,

    MAX_FONTS
};

extern PIGFont *fonts[MAX_FONTS];

void InitializeFonts(void);
void DrawText(PIGFont *font, int x, int y, float scale, const char *msg);
void DrawCharacter(PIGFont *font, int x, int y, float scale, uint8_t character);