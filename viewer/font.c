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

#include "main.h"
#include "font.h"

#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilut.h>

/*  TAB Format Specification        */
typedef struct TABIndex {
    // C0035A00 02001E00

    int32_t unknown0;

    uint16_t width;
    uint16_t height;
} TABIndex;

PLMesh *font_mesh;

void DrawText(PIGFont *font, int x, int y, const char *msg) {
    if(x < 0 || x > g_state.width || y < 0 || y > g_state.height) {
        return;
    }

    unsigned int num_chars = (unsigned int)strlen(msg);
    if(!num_chars) {
        return;
    }

    glBindTexture(GL_TEXTURE0, font->texture);

    // Very inefficient but draw each char individually for now...
    for(unsigned int i = 0; i < num_chars; i++) {
        plClearMesh(font_mesh);

        plSetMeshVertexPosition2f(font_mesh, 0, 0, 0);
        plSetMeshVertexPosition2f(font_mesh, 1, 0, 0);
        plSetMeshVertexPosition2f(font_mesh, 2, 0, 0);
        plSetMeshVertexPosition2f(font_mesh, 3, 0, 0);

        plUploadMesh(font_mesh);
        plDrawMesh(font_mesh);
    }

    glBindTexture(GL_TEXTURE0, 0);
}

PIGFont *CreateFont(const char *path, const char *tab_path) {
    PIGFont *font = (PIGFont*)malloc(sizeof(PIGFont));
    if(!font) {
        PRINT_ERROR("Failed to allocate memory for PIGFont!\n");
    }

    FILE *file = fopen(path, "rb");
    if(!file) {
        PRINT_ERROR("Failed to load file %s!\n", path);
    }

    fseek(file, 16, SEEK_SET);

    TABIndex indices[128];
    font->num_chars = (unsigned int)fread(indices, sizeof(TABIndex), 128, file);
    if(font->num_chars == 0) {
        PRINT_ERROR("Invalid number of characters for font! (%s)\n", path);
    }

    ILuint image = ilGenImage();
    ilBindImage(image);

    if(!ilLoadImage(path)) {
        PRINT_ERROR("Failed to load image! (%s)\n", path);
    }

    font->width = (unsigned int)ilGetInteger(IL_IMAGE_WIDTH);
    font->height = (unsigned int)ilGetInteger(IL_IMAGE_HEIGHT);
    font->texture = ilutGLBindTexImage();

    for(unsigned int i = 0; i < font->num_chars; i++) {
        font->chars[i].character = (unsigned char)(33 + i);
        font->chars[i].width = indices[i].width;
        font->chars[i].height = indices[i].height;

        if(i > 0) {
            font->chars[i].x = (font->chars[i - 1].width + font->chars[i - 1].x);
            if(font->chars[i].x > font->width) {
                font->chars[i].y += font->chars[i].height;
            }
        }
    }

    ilDeleteImage(image);

    fclose(file);

    return font;
}

PIGFont *fonts[MAX_FONTS];

void InitializeFonts(void) {
    font_mesh = plCreateMesh(PL_PRIMITIVE_QUADS, PL_DRAW_DYNAMIC, 2, 4);
    if(!font_mesh) {
        PRINT_ERROR("Failed to create font mesh!\n");
    }

    if(g_state.is_psx) {
        // todo
    } else {
        fonts[FONT_BIG]         = CreateFont("./FEText/BIG.BMP", "./FEText/BIG.tab");
        fonts[FONT_BIG_CHARS]   = CreateFont("./FEText/BigChars.BMP", "./FEText/BigChars.tab");
        fonts[FONT_CHARS2]      = CreateFont("./FEText/Chars2.bmp", "./FEText/CHARS2.tab");
        fonts[FONT_CHARS3]      = CreateFont("./FEText/CHARS3.BMP", "./FEText/chars3.tab");
        fonts[FONT_GAME_CHARS]  = CreateFont("./FEText/GameChars.bmp", "./FEText/GameChars.tab");
        fonts[FONT_SMALL]       = CreateFont("./FEText/SMALL.BMP", "./FEText/SMALL.tab");
    }
}