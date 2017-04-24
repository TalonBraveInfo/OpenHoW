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
#include <IL/ilu.h>
#include <IL/ilut.h>

/*  TAB Format Specification        */
typedef struct TABIndex {
    // C0035A00 02001E00

    int32_t unknown0;

    uint16_t width;
    uint16_t height;
} TABIndex;

PLMesh *font_mesh;

void DrawText(PIGFont *font, int x, int y, unsigned int scale) {
    if(x < 0 || x > g_state.width || y < 0 || y > g_state.height) {
        return;
    }

    plClearMesh(font_mesh);

    plSetMeshVertexPosition3f(font_mesh, 0, 0, 0, 0);
    plSetMeshVertexPosition3f(font_mesh, 1, 0, 0, 0);
    plSetMeshVertexPosition3f(font_mesh, 2, 0, 0, 0);
    plSetMeshVertexPosition3f(font_mesh, 3, 0, 0, 0);

    plUploadMesh(font_mesh);
}

PIGFont *CreatePIGFont(const char *path) {
    PIGFont *font = (PIGFont*)malloc(sizeof(PIGFont));
    if(!font) {
        PRINT_ERROR("Failed to allocate memory for PIGFont!\n");
    }

    FILE *file = fopen(path, "rb");
    if(!file) {
        PRINT_ERROR("Failed to load file %s!\n", path);
    }

    TABIndex indices[128];
    font->num_chars = (unsigned int)fread(indices, sizeof(TABIndex), 128, file);
    if(font->num_chars == 0) {
        PRINT_ERROR("Invalid number of characters for font! (%s)\n", path);
    }

    for(unsigned int i = 0; i < font->num_chars; i++) {
        font->chars[i].width = indices[i].width;
        font->chars[i].height = indices[i].height;
    }

    ILuint image = ilGenImage();
    ilBindImage(image);

    if(!ilLoadImage(path)) {
        PRINT_ERROR("Failed to load image! (%s)\n", path);
    }

    font->width = (unsigned int)ilGetInteger(IL_IMAGE_WIDTH);
    font->height = (unsigned int)ilGetInteger(IL_IMAGE_HEIGHT);
    font->texture = ilutGLBindTexImage();

    ilDeleteImage(image);

    return font;
}

void InitializeFonts(void) {
    font_mesh = plCreateMesh(PL_PRIMITIVE_TRIANGLES, PL_DRAW_DYNAMIC, 2, 4);
    if(!font_mesh) {
        PRINT_ERROR("Failed to create font mesh!\n");
    }


}