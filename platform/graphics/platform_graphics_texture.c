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

#include <PL/platform_image.h>
#include <PL/platform_console.h>

PLConsoleVariable pl_texture_anisotropy = { "gr_texture_anisotropy", "16", PL_VAR_INTEGER };

#define FREE_TEXTURE    ((unsigned int)-1)

void _plInitTextures(void) {
    pl_graphics_state.tmu = (PLTextureMappingUnit*)calloc(plGetMaxTextureUnits(), sizeof(PLTextureMappingUnit));
    memset(pl_graphics_state.tmu, 0, sizeof(PLTextureMappingUnit));
    for (unsigned int i = 0; i < plGetMaxTextureUnits(); i++) {
        pl_graphics_state.tmu[i].current_envmode = PL_TEXTUREMODE_REPLACE;
    }

    pl_graphics_state.max_textures  = 1024;
    pl_graphics_state.textures      = (PLTexture**)malloc(sizeof(PLTexture*) * pl_graphics_state.max_textures);
    memset(pl_graphics_state.textures, 0, sizeof(PLTexture*) * pl_graphics_state.max_textures);
    pl_graphics_state.num_textures  = 0;

    plRegisterConsoleVariables(&pl_texture_anisotropy, 1);
}

void _plShutdownTextures(void) {
    if(pl_graphics_state.tmu) {
        free(pl_graphics_state.tmu);
    }

    if(pl_graphics_state.textures) {
        for(PLTexture **texture = pl_graphics_state.textures;
            texture < pl_graphics_state.textures + pl_graphics_state.num_textures; ++texture) {
            if ((*texture)) {
                plDeleteTexture((*texture), true);
            }
        }
        free(pl_graphics_state.textures);
    }
}

/////////////////////////////////////////////////////

unsigned int plGetMaxTextureSize(void) {
    if (pl_graphics_state.hw_maxtexturesize != 0) {
        return pl_graphics_state.hw_maxtexturesize;
    }

#if defined (PL_MODE_OPENGL) || defined (PL_MODE_OPENGL_CORE)
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&pl_graphics_state.hw_maxtexturesize);
#elif defined (PL_MODE_GLIDE)
    grGet(GR_MAX_TEXTURE_SIZE, sizeof(pl_graphics_state.hw_maxtexturesize), &pl_graphics_state.hw_maxtexturesize);
#else // Software
    pl_graphics_state.hw_maxtexturesize = 4096;
#endif

    return pl_graphics_state.hw_maxtexturesize;
}

/////////////////////////////////////////////////////

#if defined(PL_MODE_OPENGL)

unsigned int _plTranslateTextureUnit(unsigned int target) {
    unsigned int out = GL_TEXTURE0 + target;
    if (out > (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)) {
        plGraphicsLog("Attempted to select an invalid texture image unit! (%i)\n", target);
    }
    return out;
}

unsigned int _plTranslateTextureTarget(PLTextureTarget target) {
    switch (target) {
        default:
        case PL_TEXTURE_2D: return GL_TEXTURE_2D;
        case PL_TEXTURE_1D: return GL_TEXTURE_1D;
        case PL_TEXTURE_3D: return GL_TEXTURE_3D;
    }
}

unsigned int _plTranslateColourFormat(PLColourFormat format) {
    switch(format) {
#if defined(PL_MODE_OPENGL) || defined(VL_MODE_OPENGL_CORE)
        default:
        case PL_COLOURFORMAT_RGB:   return GL_RGB;
        case PL_COLOURFORMAT_RGBA:  return GL_RGBA;
        case PL_COLOURFORMAT_BGR:   return GL_BGR;
        case PL_COLOURFORMAT_BGRA:  return GL_BGRA;
#elif defined(VL_MODE_GLIDE)
        default:
        case PL_COLOURFORMAT_RGBA:  return GR_COLORFORMAT_RGBA;
        case PL_COLOURFORMAT_BGRA:  return GR_COLORFORMAT_BGRA;
        case PL_COLOURFORMAT_ARGB:  return GR_COLORFORMAT_ARGB;
        case PL_COLOURFORMAT_ABGR:  return GR_COLORFORMAT_ABGR;
#endif
    }
}

unsigned int _plTranslateTextureFormat(PLImageFormat format) {
    switch (format) {
        default:
        case PL_IMAGEFORMAT_RGB4:         return GL_RGB4;
        case PL_IMAGEFORMAT_RGBA4:        return GL_RGBA4;
        case PL_IMAGEFORMAT_RGB5:         return GL_RGB5;
        case PL_IMAGEFORMAT_RGB5A1:       return GL_RGB5_A1;
#if defined(PL_MODE_OPENGL_CORE)
        case PL_IMAGEFORMAT_RGB565:       return GL_RGB565;
#endif
        case PL_IMAGEFORMAT_RGB8:         return GL_RGB;
        case PL_IMAGEFORMAT_RGBA8:        return GL_RGBA8;
        case PL_IMAGEFORMAT_RGBA12:       return GL_RGBA12;
        case PL_IMAGEFORMAT_RGBA16:       return GL_RGBA16;
#if defined(PL_MODE_OPENGL_CORE)
        case PL_IMAGEFORMAT_RGBA16F:      return GL_RGBA16F;
#endif
        case PL_IMAGEFORMAT_RGBA_DXT1:    return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case PL_IMAGEFORMAT_RGB_DXT1:     return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case PL_IMAGEFORMAT_RGBA_DXT3:    return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case PL_IMAGEFORMAT_RGBA_DXT5:    return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    }
}

unsigned int _plTranslateTextureStorageFormat(PLDataFormat format) {
    switch (format) {
        default:
        case PL_UNSIGNED_BYTE:              return GL_UNSIGNED_BYTE;
        case PL_UNSIGNED_INT_8_8_8_8_REV:   return GL_UNSIGNED_INT_8_8_8_8_REV;
    }
}

unsigned int _plTranslateTextureEnvironmentMode(PLTextureEnvironmentMode mode) {
    switch (mode) {
        default:
        case PL_TEXTUREMODE_ADD:        return GL_ADD;
        case PL_TEXTUREMODE_MODULATE:   return GL_MODULATE;
        case PL_TEXTUREMODE_DECAL:      return GL_DECAL;
        case PL_TEXTUREMODE_BLEND:      return GL_BLEND;
        case PL_TEXTUREMODE_REPLACE:    return GL_REPLACE;
        case PL_TEXTUREMODE_COMBINE:    return GL_COMBINE;
    }
}

int _plTranslateColourChannel(int channel) {
    switch(channel) {
        case PL_RED:    return GL_RED;
        case PL_GREEN:  return GL_GREEN;
        case PL_BLUE:   return GL_BLUE;
        case PL_ALPHA:  return GL_ALPHA;
        default:        return channel;
    }
}

#endif

/////////////////////////////////////////////////////

PLTexture *plCreateTexture(void) {
    PLTexture **texture = pl_graphics_state.textures; unsigned int slot;
    for(slot = 0; texture < pl_graphics_state.textures + pl_graphics_state.max_textures; ++texture, ++slot) {
        if(!(*texture)) {
            (*texture) = (PLTexture*)malloc(sizeof(PLTexture));
            if(!(*texture)) {
                _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate memory for Texture object, %d\n", sizeof(PLTexture));
                return NULL;
            }

            memset((*texture), 0, sizeof(PLTexture));
            break;
        }

        if((*texture)->id == FREE_TEXTURE) {
            break;
        }
    }

    if(texture >= pl_graphics_state.textures + pl_graphics_state.max_textures) { // ran out of available slots... ?
        PLTexture **old_mem = pl_graphics_state.textures;
        pl_graphics_state.textures = (PLTexture**)realloc(
                pl_graphics_state.textures,
                (pl_graphics_state.max_textures += 512) * sizeof(PLTexture)
        );
        if(!pl_graphics_state.textures) {
            _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate %d bytes!\n",
                           pl_graphics_state.max_textures * sizeof(PLTexture));
            pl_graphics_state.textures = old_mem;
            pl_graphics_state.max_textures -= 512;
        }
        memset(pl_graphics_state.textures + (sizeof(PLTexture) * (pl_graphics_state.max_textures - 512)), 0, sizeof(PLTexture*) * 512);
        return plCreateTexture();
    }

    (*texture)->id         = slot;
    (*texture)->format     = PL_IMAGEFORMAT_RGBA8;
    (*texture)->width      = 8;
    (*texture)->height     = 8;

#if defined(PL_MODE_OPENGL)
    glGenTextures(1, &(*texture)->gl_id);
#endif

    return (*texture);
}

void plDeleteTexture(PLTexture *texture, bool force) {
    plAssert(texture);

#if defined(PL_MODE_OPENGL)
    glDeleteTextures(1, &texture->id);
#endif

    if(!force) {
        memset(texture, 0, sizeof(PLTexture));
        texture->id = FREE_TEXTURE;
        return;
    }

    pl_graphics_state.textures[texture->id] = NULL;
    free(texture);

#if 0 // our original c++ implementation
    auto tex = _pl_graphics_textures.begin();
    while(tex != _pl_graphics_textures.end()) {
        if(tex->second == texture) {
            glDeleteTextures(1, &texture->id);

            delete tex->second;
            _pl_graphics_textures.erase(tex);
            return;
        }
        ++tex;
    }
#endif
}

/////////////////////////////////////////////////////

PLTexture *plGetCurrentTexture(unsigned int tmu) {
    for(PLTexture **texture = pl_graphics_state.textures;
        texture < pl_graphics_state.textures + pl_graphics_state.num_textures; ++texture) {
        if(pl_graphics_state.tmu[tmu].current_texture == (*texture)->id) {
            return (*texture);
        }
    }
    return NULL;
}

unsigned int plGetMaxTextureUnits(void) {
    if (pl_graphics_state.hw_maxtextureunits != 0) {
        return pl_graphics_state.hw_maxtextureunits;
    }

#ifdef PL_MODE_OPENGL
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&pl_graphics_state.hw_maxtextureunits);
#elif defined (VL_MODE_GLIDE)
    grGet(GR_NUM_TMU, sizeof(param), (FxI32*)graphics_state.hw_maxtextureunits);
#else
    pl_graphics_state.hw_maxtextureunits = PLGRAPHICS_SW_MAX_TEXTUREUNITS;
#endif

    return pl_graphics_state.hw_maxtextureunits;
}

unsigned int plGetCurrentTextureUnit(void) {
    return pl_graphics_state.current_textureunit;
}

void plSetTextureUnit(unsigned int target) {
    if (target == pl_graphics_state.current_textureunit)
        return;

    if (target > plGetMaxTextureUnits()) {
        plGraphicsLog("Attempted to select a texture image unit beyond what's supported by your hardware! (%i)\n",
                      target);
        return;
    }

#if defined (PL_MODE_OPENGL)
    glActiveTexture(_plTranslateTextureUnit(target));
#endif

    pl_graphics_state.current_textureunit = target;
}

unsigned int plGetMaxTextureAnistropy(void) {
    if (pl_graphics_state.hw_maxtextureanistropy != 0) {
        return pl_graphics_state.hw_maxtextureanistropy;
    }

#if defined (PL_MODE_OPENGL)
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat*)&pl_graphics_state.hw_maxtextureanistropy);
#else
    pl_graphics_state.hw_maxtextureanistropy = 1;
#endif

    return pl_graphics_state.hw_maxtextureanistropy;
}

// todo, hook this up with var
void plSetTextureAnisotropy(PLTexture *texture, unsigned int amount) {
#if defined (PL_MODE_OPENGL) && !defined(PL_MODE_OPENGL_CORE)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (int) amount);
#endif
}

#if defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

void plBindTexture(PLTexture *texture) {
    plAssert(texture);

    if (texture->id == pl_graphics_state.tmu[plGetCurrentTextureUnit()].current_texture) {
        return;
    }

#if defined (PL_MODE_OPENGL)
    glBindTexture(GL_TEXTURE_2D, texture->id);
#endif

    pl_graphics_state.tmu[plGetCurrentTextureUnit()].current_texture = texture->id;
}

void plSetTextureFilter(PLTexture *texture, PLTextureFilter filter) {
    plAssert(texture);

    plBindTexture(texture);

#ifdef PL_MODE_OPENGL
    unsigned int filtermin, filtermax;
    switch (filter) {
        default:
        case PL_TEXTUREFILTER_NEAREST:
            filtermax = GL_NEAREST;
            filtermin = GL_NEAREST;
            break;
        case PL_TEXTUREFILTER_LINEAR:
            filtermax = GL_LINEAR;
            filtermin = GL_LINEAR;
            break;
        case PL_TEXTUREFILTER_MIPMAP_LINEAR:
            filtermax = GL_LINEAR;
            filtermin = GL_LINEAR_MIPMAP_LINEAR;
            break;
        case PL_TEXTUREFILTER_MIPMAP_NEAREST:
            filtermax = GL_NEAREST;
            filtermin = GL_NEAREST_MIPMAP_NEAREST;
            break;
        case PL_TEXTUREFILTER_MIPMAP_LINEAR_NEAREST:
            filtermax = GL_LINEAR;
            filtermin = GL_LINEAR_MIPMAP_NEAREST;
            break;
        case PL_TEXTUREFILTER_MIPMAP_NEAREST_LINEAR:
            filtermax = GL_NEAREST;
            filtermin = GL_NEAREST_MIPMAP_LINEAR;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtermin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtermax);
#elif defined(VL_MODE_GLIDE)
    // NEAREST > GR_TEXTUREFILTER_POINT_SAMPLED
    // LINEAR > GR_TEXTUREFILTER_BILINEAR
    // todo, glide implementation
#endif
}

void plSetTextureEnvironmentMode(PLTextureEnvironmentMode mode) {
    if (pl_graphics_state.tmu[plGetCurrentTextureUnit()].current_envmode == mode)
        return;

#if defined(PL_MODE_OPENGL) && !defined(PL_MODE_OPENGL_CORE)
    glTexEnvi
            (
                    GL_TEXTURE_ENV,
                    GL_TEXTURE_ENV_MODE,
                    _plTranslateTextureEnvironmentMode(mode)
            );
#elif defined(PL_MODE_OPENGL_CORE)
    // todo
#endif

    pl_graphics_state.tmu[plGetCurrentTextureUnit()].current_envmode = mode;
}

/////////////////////

bool plUploadTextureImage(PLTexture *texture, const PLImage *upload) {
    plAssert(texture);

    plBindTexture(texture);

    texture->width      = upload->width;
    texture->height     = upload->height;
    texture->format     = upload->format;
    texture->size       = upload->size;

#if defined(PL_MODE_OPENGL)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    unsigned int levels = upload->levels;
    if(!levels) {
        levels = 1;
    }

    unsigned int format = _plTranslateTextureFormat(upload->format);
    for(unsigned int i = 0; i < levels; i++) {
        if (plIsCompressedImageFormat(upload->format)) {
            glCompressedTexSubImage2D
                    (
                            GL_TEXTURE_2D,
                            i,
                            upload->x, upload->y,
                            upload->width / (unsigned int)pow(2, i),
                            upload->height / (unsigned int)pow(2, i),
                            format,
                            upload->size,
                            upload->data[i]
                    );
        } else {
            glTexImage2D
                    (
                            GL_TEXTURE_2D,
                            i,
                            upload->x, upload->y,
                            upload->width / (PLuint)pow(2, i),
                            upload->height / (PLuint)pow(2, i),
                            _plTranslateColourFormat(upload->colour_format),
                            GL_UNSIGNED_BYTE,
                            upload->data[i]
                    );
        }
    }

    if(plIsGraphicsStateEnabled(PL_CAPABILITY_GENERATEMIPMAP) && (levels > 1)) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
#endif

    return true;
}

void plSwizzleTexture(PLTexture *texture, int r, int g, int b, int a) {
    plAssert(texture);

    plBindTexture(texture);

    if(_PLGL_VERSION(3,3)) {
        int swizzle[] = {
                _plTranslateColourChannel(r),
                _plTranslateColourChannel(g),
                _plTranslateColourChannel(b),
                _plTranslateColourChannel(a)
        };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    } else {
        // todo, software implementation
    }
}