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

typedef struct PLImage PLImage;

// Texture Environment Modes
typedef enum PLTextureEnvironmentMode {
    PL_TEXTUREMODE_ADD,
    PL_TEXTUREMODE_MODULATE,
    PL_TEXTUREMODE_DECAL,
    PL_TEXTUREMODE_BLEND,
    PL_TEXTUREMODE_REPLACE,
    PL_TEXTUREMODE_COMBINE
} PLTextureEnvironmentMode;

typedef enum PLTextureClamp {
#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    VL_TEXTURECLAMP_CLAMP = GL_CLAMP_TO_EDGE,
    VL_TEXTURECLAMP_WRAP = GL_REPEAT,
#elif defined (VL_MODE_GLIDE)
    VL_TEXTURECLAMP_CLAMP	= GR_TEXTURECLAMP_CLAMP,
    VL_TEXTURECLAMP_WRAP	= GR_TEXTURECLAMP_WRAP,
#else
    VL_TEXTURECLAMP_CLAMP,
    VL_TEXTURECLAMP_WRAP,
#endif
} PLTextureClamp;

typedef enum PLTextureFilter {
    PL_TEXTUREFILTER_MIPMAP_NEAREST,        // GL_NEAREST_MIPMAP_NEAREST
    PL_TEXTUREFILTER_MIPMAP_LINEAR,         // GL_LINEAR_MIPMAP_LINEAR

    PL_TEXTUREFILTER_MIPMAP_LINEAR_NEAREST,    // GL_LINEAR_MIPMAP_NEAREST
    PL_TEXTUREFILTER_MIPMAP_NEAREST_LINEAR,    // GL_NEAREST_MIPMAP_LINEAR

    PL_TEXTUREFILTER_NEAREST,       // Nearest filtering
    PL_TEXTUREFILTER_LINEAR         // Linear filtering
} PLTextureFilter;

typedef enum PLTextureTarget {
    PL_TEXTURE_1D,
    PL_TEXTURE_2D,
    PL_TEXTURE_3D
} PLTextureTarget;

enum PLTextureFlag {
    PL_TEXTUREFLAG_PRESERVE = (1 << 0)
};

typedef struct PLTextureMappingUnit {
    bool active;

    PLuint current_texture;
    PLuint current_capabilities;

    PLTextureEnvironmentMode current_envmode;
} PLTextureMappingUnit;

typedef struct PLTexture {
    unsigned int id;

#if defined(PL_MODE_OPENGL)
    unsigned int gl_id;
#endif

    unsigned int flags;

    unsigned int x, y;
    unsigned int width, height;

    unsigned int size;
    unsigned int levels;
    unsigned int crc;

    const char *path;

    PLImageFormat format;
    PLDataFormat storage;
    PLColourFormat pixel;
} PLTexture;

PL_EXTERN_C

PL_EXTERN PLTexture *plCreateTexture(void);
PL_EXTERN void plDeleteTexture(PLTexture *texture, bool force);

PL_EXTERN void plBindTexture(PLTexture *texture);

//PL_EXTERN PLresult plUploadTextureData(PLTexture *texture, const PLTextureInfo *upload);
PL_EXTERN bool plUploadTextureImage(PLTexture *texture, const PLImage *upload);

PL_EXTERN PLuint plGetMaxTextureSize(void);
PL_EXTERN PLuint plGetMaxTextureUnits(void);
PL_EXTERN PLuint plGetMaxTextureAnistropy(void);

PL_EXTERN void plSetTextureFilter(PLTexture *texture, PLTextureFilter filter);
PL_EXTERN void plSetTextureAnisotropy(PLTexture *texture, unsigned int amount);

PL_EXTERN void plSetTextureUnit(unsigned int target);
PL_EXTERN void plSetTextureEnvironmentMode(PLTextureEnvironmentMode mode);

PL_EXTERN const PLchar *plPrintTextureMemoryUsage(void);

PL_EXTERN_C_END