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

#include "graphics_private.h"

#include <PL/platform_graphics_font.h>
#include <PL/platform_filesystem.h>

/////////////////////////////////////////////////////////////////////////////////////
// FONT PARSER

#define _PLFONT_MAX_LENGTH  2048
#define _PLFONT_MIN_LENGTH  10

#define _PLFONT_MAX_LINE    256

struct {
    char buffer[_PLFONT_MAX_LENGTH];
    char line_buffer[_PLFONT_MAX_LINE];

    unsigned int position;
    unsigned int line, line_position;
    unsigned int length;
} _pl_font;

void _plResetFontParser(void) {
    memset(&_pl_font, 0, sizeof(_pl_font));
}

void _plNextFontLine(void) {
    _pl_font.line++;
    _pl_font.line_position = 0;
}

bool _plFontEOF(void) {
    if(_pl_font.position >= _pl_font.length) {
        return true;
    }

    return false;
}

void _plSkipFontComment(void) {
    while(!_plFontEOF() && (_pl_font.buffer[_pl_font.position] != '\n')) {
        _pl_font.position++;
    }

    _pl_font.position++;
    _plNextFontLine();
}

void _plParseFontLine(void) {
    if(_pl_font.position >= _pl_font.length) {
        return;
    }

    while(!_plFontEOF()) {
        if((_pl_font.buffer[_pl_font.position] == '-') && ((_pl_font.buffer[_pl_font.position + 1] == '-'))) {
            _plSkipFontComment();
            continue;
        }

        if((_pl_font.line_position == 0) && (_pl_font.buffer[_pl_font.position] == '\n')) {
            _pl_font.position++;
            continue;
        }

        if(_pl_font.buffer[_pl_font.position] == '\t') {
            _pl_font.position++;
            continue;
        }

        if(_pl_font.buffer[_pl_font.position] == '\n') {
            _pl_font.line_buffer[_pl_font.line_position + 1] = '\0';

            _plNextFontLine();
            _pl_font.position++;
            break;
        }

        _pl_font.line_buffer[_pl_font.line_position] = _pl_font.buffer[_pl_font.position];
        _pl_font.position++; _pl_font.line_position++;
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// BITMAP FONT RENDERING

#define _PLFONT_FORMAT_VERSION  1

PLBitmapFont *plCreateBitmapFont(const char *path) {
    _plResetFontParser();

    FILE *file = fopen(path, "r");
    if(!file) {
        _plReportError(PL_RESULT_FILEPATH, "Failed to open %s!\n", path);
        return NULL;
    }

    _pl_font.length = (unsigned int)fread(_pl_font.buffer, 1, _PLFONT_MAX_LENGTH, file);
    fclose(file);

    if(_pl_font.length < _PLFONT_MIN_LENGTH) {
        _plReportError(PL_RESULT_FILESIZE, "Invalid length, %d, for %s!\n", _pl_font.length, path);
        return NULL;
    }

    _plParseFontLine();
    if(!strncmp(_pl_font.line_buffer, "VERSION ", 8)) {
        long version = strtol(_pl_font.line_buffer + 8, NULL, 0);
        if (version <= 0 || version > _PLFONT_FORMAT_VERSION) {
            _plReportError(PL_RESULT_FILEVERSION, "Expected version %d, received %d, for %s!\n",
                           _PLFONT_FORMAT_VERSION, version, path);
            return NULL;
        }
    } else {
        _plReportError(PL_RESULT_FILEVERSION, "Failed to fetch version for %s!\n", path);
        return NULL;
    }

    _plParseFontLine();
    if(!plFileExists(_pl_font.line_buffer)) {
        _plReportError(PL_RESULT_FILEPATH, "Failed to find texture at %s, for %s!\n", _pl_font.line_buffer, path);
        return NULL;
    }

    char image_path[PL_SYSTEM_MAX_PATH] = { 0 };
    strncpy(image_path, _pl_font.line_buffer, sizeof(image_path));

    PLBitmapFont *font = (PLBitmapFont*)malloc(sizeof(PLBitmapFont));
    if(!font) {
        _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate memory for BitmapFont, %d!\n", sizeof(PLBitmapFont));
        return NULL;
    }
    memset(font, 0, sizeof(PLBitmapFont));

    bool enable_filter = false;
    while(!_plFontEOF()) {
        _plParseFontLine();

        if(!strncmp(_pl_font.line_buffer, "FILTER ", 7)) {
            if(_pl_font.line_buffer[8] == '1') {
                enable_filter = true;
            }
            continue;
        }

        int8_t character = _pl_font.line_buffer[0];
        if(character == ' ') {
            continue;
        }

        char *pos;
        font->chars[character].x = (int)strtol(_pl_font.line_buffer + 2, &pos, 10);
        font->chars[character].y = (int)strtol(pos, &pos, 10);
        font->chars[character].w = (int)strtoul(pos, &pos, 10);
        font->chars[character].h = (int)strtoul(pos, &pos, 10);

#if 1
        printf("CHAR(%c) X(%d) Y(%d) W(%d) H(%d)\n",
               character,
               font->chars[character].x,
               font->chars[character].y,
               font->chars[character].w,
               font->chars[character].h
        );
#endif
    }

    PLImage image;
    if(plLoadImage(_pl_font.line_buffer, &image) != PL_RESULT_SUCCESS) {
        plDeleteBitmapFont(font);
        return NULL;
    }

    if(!(font->texture = plCreateTexture())) {
        plDeleteBitmapFont(font);
        plFreeImage(&image);
        return NULL;
    }

#if defined(PL_MODE_OPENGL)
    glBindTexture(GL_TEXTURE_RECTANGLE, font->texture->id);

    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
    if(enable_filter) {
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    glTexImage2D(
            GL_TEXTURE_RECTANGLE,
            0,
            _plTranslateColourFormat(image.colour_format),
            image.width,
            image.height,
            0,
            _plTranslateTextureFormat(image.format),
            GL_UNSIGNED_BYTE,
            image.data[0]
    );
#endif

//    plFreeImage(&image);

    return font;
}

void plDeleteBitmapFont(PLBitmapFont *font) {
    if(!font) {
        return;
    }

    plDeleteTexture(font->texture, false);

    free(font);
}

void plDrawCharacter(PLBitmapFont *font, int x, int y, float scale, int8_t character) {
    if((character == ' ') || (character == '\n') || (character == '\t') ||
            !font->chars[character].w || !font->chars[character].h) {
        return;
    }

    static PLMesh *mesh = NULL;
    if(!mesh) {
        if(!(mesh = plCreateMesh(
                PLMESH_TRIANGLES,
                PL_DRAW_IMMEDIATE,
                2, 6
        ))) {
            return;
        }
    }

    float w = font->chars[character].w * scale;
    float h = font->chars[character].h * scale;

    plClearMesh(mesh);

    plSetMeshVertexPosition2f(mesh, 0, x, y + h);
    plSetMeshVertexPosition2f(mesh, 1, x, y);
    plSetMeshVertexPosition2f(mesh, 2, x + w, y);

    plSetMeshVertexPosition2f(mesh, 3, x, y + h);
    plSetMeshVertexPosition2f(mesh, 4, x + w, y);
    plSetMeshVertexPosition2f(mesh, 5, x + w, y + h);

    plSetMeshVertexST(mesh, 0, 0, 0);
    plSetMeshVertexST(mesh, 1, 0, 1);
    plSetMeshVertexST(mesh, 2, 1, 1);

    plSetMeshVertexST(mesh, 3, 0, 0);
    plSetMeshVertexST(mesh, 4, 1, 1);
    plSetMeshVertexST(mesh, 5, 1, 0);

    plUploadMesh(mesh);
    plDrawMesh(mesh);
}

void plDrawString(PLBitmapFont *font, int x, int y, float scale, const char *msg) {
    plAssert(scale > 0);

    if(x < 0 || y > pl_graphics_state.viewport_width || y < 0 || y > pl_graphics_state.viewport_height) {
        return;
    }

    unsigned int length = (unsigned int)strlen(msg);
    if(length == 0) {
        return;
    }

    for(unsigned int i = 0; i < length; i++) {

    }
}