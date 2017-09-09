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

#include <PL/platform_graphics.h>

typedef struct PLGraphicsState {
    PLCullMode current_cullmode;

    PLColour current_clearcolour;
    PLColour current_colour;        // Current global colour.

    unsigned int current_capabilities;    // Enabled capabilities.
    unsigned int current_textureunit;

    // Textures

    PLTextureMappingUnit    *tmu;
    PLTexture               **textures;

    unsigned int num_textures;
    unsigned int max_textures;

    // Shader states

    unsigned int current_program;

    // Hardware / Driver information

    const char *hw_vendor;
    const char *hw_renderer;
    const char *hw_version;
    const char *hw_extensions;

    unsigned int hw_maxtexturesize;
    unsigned int hw_maxtextureunits;
    unsigned int hw_maxtextureanistropy;

    // Lighting

    unsigned int num_lights;

    // Cameras

    PLCamera **cameras;

    unsigned int num_cameras;
    unsigned int max_cameras;

    ////////////////////////////////////////

    int viewport_x, viewport_y;
    unsigned int viewport_width, viewport_height;

    bool mode_debug;
} PLGraphicsState;

PL_EXTERN_C

#if defined(PL_MODE_OPENGL)

PL_EXTERN unsigned int pl_gl_version_major;
PL_EXTERN unsigned int pl_gl_version_minor;
#define _PLGL_VERSION(maj, min) (((maj) == pl_gl_version_major && (min) <= pl_gl_version_minor) || (maj) < pl_gl_version_major)

#endif

PL_EXTERN PLGraphicsState pl_graphics_state;

unsigned int _plTranslateColourFormat(PLColourFormat format);
unsigned int _plTranslateTextureFormat(PLImageFormat format);

PL_EXTERN_C_END