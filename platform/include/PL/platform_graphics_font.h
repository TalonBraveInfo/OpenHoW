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

#include <PL/platform_graphics.h>

typedef struct PLBitmapFontChar {
    int x, y;
    unsigned int w, h;

    //char character;

    float s, t;
} PLBitmapFontChar;

#define PLFONT_MAX_CHARS    256

typedef struct PLBitmapFont {
    PLTexture *texture;

    PLBitmapFontChar chars[PLFONT_MAX_CHARS];
} PLBitmapFont;

PL_EXTERN_C

PL_EXTERN PLBitmapFont *plCreateBitmapFont(const char *path);
PL_EXTERN void plDeleteBitmapFont(PLBitmapFont *font);

PL_EXTERN void plDrawCharacter(PLBitmapFont *font, int x, int y, float scale, int8_t character);

PL_EXTERN_C_END