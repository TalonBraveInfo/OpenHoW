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