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

#include "PL/platform_graphics.h"
#include "PL/platform_log.h"
#include "PL/platform_image.h"

using namespace pl::graphics;

/*	Graphics	*/

/*	todo,
		replace glew with our own internal solution.
		make everything more scalable depending on current hw.
		decide if we're going to cut support for earlier versions of OpenGL.
		are we going to continue with Glide implementation?
		internal lighting system?? should not be shader-based; just cheap CPU solution.
		how are we going to deal with the fact that Glide doesn't support shaders? Funky SW-based shader solution?
		support for SW rendering? could be neat.
*/

PLGraphicsState pl_graphics_state;

/*	TODO:
- Add somewhere we can store tracking
data for each of these functions
- Do this in another thread if possible
- Display that data as an overlay
*/
#define    _PL_GRAPHICS_TRACK()                     \
    {                                               \
        unsigned static int _t = 0;                 \
        if(pl_graphics_state.mode_debug)            \
        {                                           \
            plGraphicsLog(" %s\n", PL_FUNCTION);    \
            _t++;                                   \
        }                                           \
    }

/*===========================
	INITIALIZATION
===========================*/

#if defined (VL_MODE_GLIDE)

#elif defined (VL_MODE_DIRECT3D)

#elif defined(PL_MODE_VULKAN)

void _plInitVulkan() {

}

#elif defined (PL_MODE_OPENGL)

PLbool pl_gl_generate_mipmap = PL_FALSE;
PLbool pl_gl_depth_texture = PL_FALSE;
PLbool pl_gl_shadow = PL_FALSE;
PLbool pl_gl_vertex_buffer_object = PL_FALSE;
PLbool pl_gl_texture_compression = PL_FALSE;
PLbool pl_gl_texture_compression_s3tc = PL_FALSE;
PLbool pl_gl_multitexture = PL_FALSE;
PLbool pl_gl_texture_env_combine = PL_FALSE;
PLbool pl_gl_texture_env_add = PL_FALSE;
PLbool pl_gl_vertex_program = PL_FALSE;
PLbool pl_gl_fragment_program = PL_FALSE;

PLuint pl_gl_version_major = 0;
PLuint pl_gl_version_minor = 0;

#define PL_GL_VERSION(maj, min) ((maj == pl_gl_version_major && min <= pl_gl_version_minor) || maj < pl_gl_version_major)

void _plInitOpenGL() {
    _PL_GRAPHICS_TRACK();

    // todo, write our own replacement for GLEW

#if 0
#if !defined(PL_MODE_OPENGL_CORE)
    PLuint err = glewInit();
    if (err != GLEW_OK) {
        plGraphicsLog("Failed to initialize GLEW!\n%s\n", glewGetErrorString(err));
    }

    // Check that the required capabilities are supported.
    if (GLEW_ARB_multitexture) pl_gl_multitexture = PL_TRUE;
    else plGraphicsLog("Video hardware incapable of multi-texturing!\n");
    if (GLEW_ARB_texture_env_combine || GLEW_EXT_texture_env_combine) pl_gl_texture_env_combine = PL_TRUE;
    else plGraphicsLog("ARB/EXT_texture_env_combine isn't supported by your hardware!\n");
    if (GLEW_ARB_texture_env_add || GLEW_EXT_texture_env_add) pl_gl_texture_env_add = PL_TRUE;
    else plGraphicsLog("ARB/EXT_texture_env_add isn't supported by your hardware!\n");
    if (GLEW_ARB_vertex_program || GLEW_ARB_fragment_program) {
        pl_gl_vertex_program = PL_TRUE;
        pl_gl_fragment_program = PL_TRUE;
    }
    else plGraphicsLog("Shaders aren't supported by this hardware!\n");
    if (GLEW_SGIS_generate_mipmap) pl_gl_generate_mipmap = PL_TRUE;
    else plGraphicsLog("Hardware mipmap generation isn't supported!\n");
    if (GLEW_ARB_depth_texture) pl_gl_depth_texture = PL_TRUE;
    else plGraphicsLog("ARB_depth_texture isn't supported by your hardware!\n");
    if (GLEW_ARB_shadow) pl_gl_shadow = PL_TRUE;
    else plGraphicsLog("ARB_shadow isn't supported by your hardware!\n");
    if (GLEW_ARB_vertex_buffer_object) pl_gl_vertex_buffer_object = PL_TRUE;
    else plGraphicsLog("Hardware doesn't support Vertex Buffer Objects!\n");

    // If HW compression isn't supported then we'll need to do
    // all of this in software later.
    if (GLEW_ARB_texture_compression) {
        if (GLEW_EXT_texture_compression_s3tc) {
            pl_gl_texture_compression_s3tc = PL_TRUE;
        }
    }
#endif
#endif

    //const PLchar *version = _plGetHWVersion();
    //pl_gl_version_major = (unsigned int) atoi(&version[0]);
    //pl_gl_version_minor = (unsigned int) atoi(&version[2]);
}

void _plShutdownOpenGL() {}

#endif

PLresult _plInitGraphics(void) {
    _PL_GRAPHICS_TRACK();

    plClearLog(PL_GRAPHICS_LOG);
    plGraphicsLog("Initializing graphics abstraction layer...\n");

    memset(&pl_graphics_state, 0, sizeof(PLGraphicsState));

#if defined (PL_MODE_OPENGL)
    _plInitOpenGL();
#elif defined (VL_MODE_GLIDE)
    _plInitGlide();
#elif defined (VL_MODE_DIRECT3D)
    _plInitDirect3D();
#endif

    pl_graphics_state.tmu = (PLTextureMappingUnit*)calloc(plGetMaxTextureUnits(), sizeof(PLTextureMappingUnit));
    memset(pl_graphics_state.tmu, 0, sizeof(PLTextureMappingUnit));
    for (unsigned int i = 0; i < plGetMaxTextureUnits(); i++) {
        pl_graphics_state.tmu[i].current_envmode = PL_TEXTUREMODE_REPLACE;
    }

    // Get any information that will be presented later.
    pl_graphics_state.hw_extensions = _plGetHWExtensions();
    pl_graphics_state.hw_renderer = _plGetHWRenderer();
    pl_graphics_state.hw_vendor = _plGetHWVendor();
    pl_graphics_state.hw_version = _plGetHWVersion();
    plGraphicsLog(" HARDWARE/DRIVER INFORMATION\n");
    plGraphicsLog("  RENDERER: %s\n", pl_graphics_state.hw_renderer);
    plGraphicsLog("  VENDOR:   %s\n", pl_graphics_state.hw_vendor);
    plGraphicsLog("  VERSION:  %s\n\n", pl_graphics_state.hw_version);

    return PL_RESULT_SUCCESS;
}

void _plShutdownGraphics(void) {
    _PL_GRAPHICS_TRACK();

#if defined (PL_MODE_OPENGL)
#elif defined (VL_MODE_GLIDE)
#elif defined (VL_MODE_DIRECT3D)
    _plShutdownDirect3D();
#endif

    delete pl_graphics_state.tmu;
}

/*===========================
	HARDWARE INFORMATION
===========================*/

const PLchar *_plGetHWExtensions(void) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    return (const PLchar *) glGetString(GL_EXTENSIONS);
    // TODO: this works differently in core; use glGetStringi instead!
#elif defined (VL_MODE_GLIDE)
    return grGetString(GR_EXTENSION);
#else
    return "";
#endif
}

const PLchar *_plGetHWRenderer(void) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    return (const PLchar *) glGetString(GL_RENDERER);
#elif defined (VL_MODE_GLIDE)
    return grGetString(GR_RENDERER);
#else
    return "";
#endif
}

const PLchar *_plGetHWVendor(void) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    return (const PLchar *) glGetString(GL_VENDOR);
#elif defined (VL_MODE_GLIDE)
    return grGetString(GR_VENDOR);
#else
    return "";
#endif
}

const PLchar *_plGetHWVersion(void) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    return (const PLchar *) glGetString(GL_VERSION);
#elif defined (VL_MODE_GLIDE)
    return grGetString(GR_VERSION);
#else
    return "";
#endif
}

bool plHWSupportsMultitexture(void) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL)
    return pl_gl_multitexture;
#elif defined(VL_MODE_GLIDE)
    return true;
#else
    return false;
#endif
}

bool plHWSupportsShaders(void) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL)
    return (pl_gl_fragment_program && pl_gl_vertex_program);
#else
    return false;
#endif
}

/*===========================
	FRAMEBUFFERS
===========================*/

void plSetClearColour(PLColour rgba) {
    if (plCompareColour(rgba, pl_graphics_state.current_clearcolour)) {
        return;
    }

#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    glClearColor(
            plByteToFloat(rgba.r),
            plByteToFloat(rgba.g),
            plByteToFloat(rgba.b),
            plByteToFloat(rgba.a)
    );
#elif defined (VL_MODE_DIRECT3D)
    // Don't need to do anything specific here, colour is set on clear call.
#else // Software
#endif

    plCopyColour(&pl_graphics_state.current_clearcolour, rgba);
}

void plClearBuffers(PLuint buffers) {
    _PL_GRAPHICS_TRACK();

#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    // Rather ugly, but translate it over to GL.
    PLuint glclear = 0;
    if(buffers & PL_BUFFER_COLOUR)  glclear |= GL_COLOR_BUFFER_BIT;
    if(buffers & PL_BUFFER_DEPTH)   glclear |= GL_DEPTH_BUFFER_BIT;
    if(buffers & PL_BUFFER_STENCIL) glclear |= GL_STENCIL_BUFFER_BIT;
    glClear(glclear);
#elif defined (VL_MODE_GLIDE)
    // Glide only supports clearing a single buffer.
    grBufferClear(
        // Convert buffer_clearcolour to something that works with Glide.
        _plConvertColour4fv(PL_COLOURFORMAT_RGBA, graphics_state.buffer_clearcolour),
        1, 1);
#elif defined (VL_MODE_DIRECT3D)
    pl_d3d_context->lpVtbl->ClearRenderTargetView(pl_d3d_context,
        pl_d3d_backbuffer,
        graphics_state.current_clearcolour
    );
#else // Software
#endif
}

/*===========================
	CAPABILITIES
===========================*/

typedef struct _PLGraphicsCapabilities {
    PLuint pl_parm, to_parm;

    const PLchar *ident;
} _PLGraphicsCapabilities;

_PLGraphicsCapabilities graphics_capabilities[] =
        {
#if defined (PL_MODE_OPENGL)
#if defined (PL_MODE_OPENGL_CORE)
                {PL_CAPABILITY_ALPHA_TEST, 0, "ALPHA_TEST"},
#else
                {PL_CAPABILITY_ALPHA_TEST, GL_ALPHA_TEST, "ALPHA_TEST"},
#endif
                {PL_CAPABILITY_BLEND, GL_BLEND, "BLEND"},
                {PL_CAPABILITY_DEPTHTEST, GL_DEPTH_TEST, "DEPTH_TEST"},
                {PL_CAPABILITY_TEXTURE_2D, GL_TEXTURE_2D, "TEXTURE_2D"},
#if defined (PL_MODE_OPENGL_CORE)
                {PL_CAPABILITY_TEXTURE_GEN_S, 0, "TEXTURE_GEN_S"},
                {PL_CAPABILITY_TEXTURE_GEN_T, 0, "TEXTURE_GEN_T"},
#else
                {PL_CAPABILITY_TEXTURE_GEN_S, GL_TEXTURE_GEN_S, "TEXTURE_GEN_S"},
                {PL_CAPABILITY_TEXTURE_GEN_T, GL_TEXTURE_GEN_T, "TEXTURE_GEN_T"},
#endif
                {PL_CAPABILITY_CULL_FACE, GL_CULL_FACE, "CULL_FACE"},
                {PL_CAPABILITY_STENCILTEST, GL_STENCIL_TEST, "STENCIL_TEST"},
                {PL_CAPABILITY_MULTISAMPLE, GL_MULTISAMPLE, "MULTISAMPLE"},
                {PL_CAPABILITY_SCISSORTEST, GL_SCISSOR_TEST, "SCISSOR_TEST"},

                {PL_CAPABILITY_GENERATEMIPMAP, 0, "GENERATE_MIPMAP"},
#else
        { PL_CAPABILITY_ALPHA_TEST, 0, "ALPHA_TEST" },
        { VL_CAPABILITY_BLEND, 0, "BLEND" },
        { PL_CAPABILITY_DEPTHTEST, 0, "DEPTH_TEST" },
        { VL_CAPABILITY_TEXTURE_2D, 0, "TEXTURE_2D" },
        { PL_CAPABILITY_TEXTURE_GEN_S, 0, "TEXTURE_GEN_S" },
        { PL_CAPABILITY_TEXTURE_GEN_T, 0, "TEXTURE_GEN_T" },
        { PL_CAPABILITY_CULL_FACE, 0, "CULL_FACE" },
        { PL_CAPABILITY_STENCILTEST, 0, "STENCIL_TEST" },
        { PL_CAPABILITY_MULTISAMPLE, 0, "MULTISAMPLE" },
#endif

                {0}
        };

bool plIsGraphicsStateEnabled(PLuint flags) {
    _PL_GRAPHICS_TRACK();

    return (bool)(flags & pl_graphics_state.current_capabilities);
}

void plEnableGraphicsStates(PLuint flags) {
    _PL_GRAPHICS_TRACK();

    if (plIsGraphicsStateEnabled(flags)) {
        return;
    }

    for (PLint i = 0; i < sizeof(graphics_capabilities); i++) {
        if (graphics_capabilities[i].pl_parm == 0) break;

        if (pl_graphics_state.mode_debug) {
            plGraphicsLog("Enabling %s\n", graphics_capabilities[i].ident);
        }

        if (flags & PL_CAPABILITY_TEXTURE_2D) {
            pl_graphics_state.tmu[pl_graphics_state.current_textureunit].active = PL_TRUE;
        }
#if defined (VL_MODE_GLIDE)
        if (flags & PL_CAPABILITY_FOG)
            // TODO: need to check this is supported...
            grFogMode(GR_FOG_WITH_TABLE_ON_FOGCOORD_EXT);
        if (flags & PL_CAPABILITY_DEPTHTEST)
            grDepthBufferMode(GR_DEPTHBUFFER_ZBUFFER);
        if (flags & PL_CAPABILITY_CULL_FACE)
            grCullMode(graphics_state.current_cullmode);
#endif

        if ((flags & graphics_capabilities[i].pl_parm) && (graphics_capabilities[i].to_parm != 0)) {
#if defined (PL_MODE_OPENGL)
            glEnable(graphics_capabilities[i].to_parm);
#elif defined (VL_MODE_GLIDE)
            grEnable(graphics_capabilities[i].to_parm);
#endif
        }

        pl_graphics_state.current_capabilities |= graphics_capabilities[i].pl_parm;
    }
}

void plDisableGraphicsStates(PLuint flags) {
    _PL_GRAPHICS_TRACK();

    if (!plIsGraphicsStateEnabled(flags)) {
        return;
    }

    for (PLint i = 0; i < sizeof(graphics_capabilities); i++) {
        if (graphics_capabilities[i].pl_parm == 0) break;

        if (pl_graphics_state.mode_debug) {
            plGraphicsLog("Disabling %s\n", graphics_capabilities[i].ident);
        }

        if (flags & PL_CAPABILITY_TEXTURE_2D) {
            pl_graphics_state.tmu[pl_graphics_state.current_textureunit].active = PL_FALSE;
        }
#if defined (VL_MODE_GLIDE)
        if (flags & PL_CAPABILITY_FOG)
            grFogMode(GR_FOG_DISABLE);
        if (flags & PL_CAPABILITY_DEPTHTEST)
            grDepthBufferMode(GR_DEPTHBUFFER_DISABLE);
        if (flags & PL_CAPABILITY_CULL_FACE)
            grCullMode(graphics_state.current_cullmode);
#endif

        if ((flags & graphics_capabilities[i].pl_parm) && (graphics_capabilities[i].to_parm != 0)) {
#if defined (PL_MODE_OPENGL)
            glDisable(graphics_capabilities[i].to_parm);
#elif defined (VL_MODE_GLIDE)
            grDisable(graphics_capabilities[i].to_parm);
#endif
        }

        pl_graphics_state.current_capabilities &= ~graphics_capabilities[i].pl_parm;
    }
}

/*===========================
	DRAW
===========================*/

void plSetBlendMode(PLBlend modea, PLBlend modeb) {
    _PL_GRAPHICS_TRACK();

#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    glBlendFunc(modea, modeb);
#elif defined (VL_MODE_GLIDE)
    grAlphaBlendFunction(modea, modeb, modea, modeb);
#endif
}

void plSetCullMode(PLCullMode mode) {
    _PL_GRAPHICS_TRACK();

    if (mode == pl_graphics_state.current_cullmode)
        return;
#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    glCullFace(GL_BACK);
    switch (mode) {
        default:
        case PL_CULL_NEGATIVE:
            glFrontFace(GL_CW);
            break;
        case PL_CULL_POSTIVE:
            glFrontFace(GL_CCW);
            break;
    }
#elif defined (VL_MODE_DIRECT3D)
    // todo, create new render state and somehow get the properties of the
    // current but update them to reflect the new cull mode.

    vl_d3d_context->lpVtbl->RSSetState(vl_d3d_context, vl_d3d_state);
#endif
    pl_graphics_state.current_cullmode = mode;
}

/*===========================
	SHADERS
===========================*/

unsigned int plGetCurrentShaderProgram(void) {
    return pl_graphics_state.current_program;
}

void plEnableShaderProgram(unsigned int program) {
    _PL_GRAPHICS_TRACK();

    if (program == pl_graphics_state.current_program) {
        return;
    }

#if defined (PL_MODE_OPENGL)
    glUseProgram(program);
#endif

    pl_graphics_state.current_program = program;
}

void plDisableShaderProgram(unsigned int program) {
    _PL_GRAPHICS_TRACK();

    if(program != pl_graphics_state.current_program) {
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUseProgram(0);
#endif

    pl_graphics_state.current_program = 0;
}

/*===========================
	TEXTURES
===========================*/

PLuint _plTranslateTextureUnit(PLuint target) {
    _PL_GRAPHICS_TRACK();

#if defined (PL_MODE_OPENGL) || defined(VL_MODE_OPENGL_CORE)
    PLuint out = GL_TEXTURE0 + target;
    if (out > (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1))
        plGraphicsLog("Attempted to select an invalid texture image unit! (%i)\n", target);
    return out;
#else
    return 0;
#endif
}

PLuint _plTranslateTextureStorageFormat(PLDataFormat format) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL) || defined(VL_MODE_OPENGL_CORE)
    switch (format) {
        default:
        case PL_UNSIGNED_BYTE:              return GL_UNSIGNED_BYTE;
        case PL_UNSIGNED_INT_8_8_8_8_REV:   return GL_UNSIGNED_INT_8_8_8_8_REV;
    }
#else
    return format;
#endif
}

PLuint _plTranslateTextureTarget(PLTextureTarget target) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    switch (target) {
        default:
        case PL_TEXTURE_2D: return GL_TEXTURE_2D;
        case PL_TEXTURE_1D: return GL_TEXTURE_1D;
        case PL_TEXTURE_3D: return GL_TEXTURE_3D;
    }
#else
    return (PLuint)format;
#endif
}

unsigned int _plTranslateTextureEnvironmentMode(PLTextureEnvironmentMode mode) {
#if defined (PL_MODE_OPENGL) && !defined (VL_MODE_OPENGL_CORE)
    switch (mode) {
        default:
        case PL_TEXTUREMODE_ADD:        return GL_ADD;
        case PL_TEXTUREMODE_MODULATE:   return GL_MODULATE;
        case PL_TEXTUREMODE_DECAL:      return GL_DECAL;
        case PL_TEXTUREMODE_BLEND:      return GL_BLEND;
        case PL_TEXTUREMODE_REPLACE:    return GL_REPLACE;
        case PL_TEXTUREMODE_COMBINE:    return GL_COMBINE;
    }
#elif defined(PL_MODE_OPENGL_CORE)
    return 0; // todo
#elif defined (VL_MODE_GLIDE)
#elif defined (VL_MODE_DIRECT3D)
#else
    // No translation if we're doing this in SW.
    return (PLuint)format;
#endif
}

unsigned int _plTranslateColourFormat(PLColourFormat format) {
    _PL_GRAPHICS_TRACK();

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
#elif defined(VL_MODE_DIRECT3D)
        // todo
#else
        default:    return format;
#endif
    }
}

unsigned int _plTranslateTextureFormat(PLImageFormat format) {
    _PL_GRAPHICS_TRACK();

#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
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
#elif defined (VL_MODE_GLIDE)
#elif defined (VL_MODE_DIRECT3D)
#else
    // No translation if we're doing this in SW.
    return (PLuint)format;
#endif
}

PLbool _plIsCompressedTextureFormat(PLImageFormat format) {
    _PL_GRAPHICS_TRACK();

    switch (format) {
        default:    return PL_FALSE;
        case PL_IMAGEFORMAT_RGBA_DXT1:
        case PL_IMAGEFORMAT_RGBA_DXT3:
        case PL_IMAGEFORMAT_RGBA_DXT5:
        case PL_IMAGEFORMAT_RGB_DXT1:
        case PL_IMAGEFORMAT_RGB_FXT1:
            return PL_TRUE;
    }
}

std::unordered_map<PLuint, PLTexture*> _pl_graphics_textures;

PLTexture *plCreateTexture(void) {
    _PL_GRAPHICS_TRACK();

    PLTexture *texture = new PLTexture;
    memset(texture, 0, sizeof(PLTexture));
    texture->format = PL_IMAGEFORMAT_RGBA8;
    texture->width = 8;
    texture->height = 8;

#if defined(PL_MODE_OPENGL)
    glGenTextures(1, &texture->id);
#else
    texture->id = _pl_graphics_textures.count + 1;
#endif

    _pl_graphics_textures.emplace(texture->id, texture);

    return texture;
}

void plDeleteTexture(PLTexture *texture, PLbool force) {
    _PL_GRAPHICS_TRACK();

    if(!texture || ((texture->flags & PL_TEXTUREFLAG_PRESERVE) && !force)) {
        return;
    }

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
}

PLint _plTranslateColourChannel(PLint channel) {
    _PL_GRAPHICS_TRACK();

    switch(channel) {
        case PL_RED:    return GL_RED;
        case PL_GREEN:  return GL_GREEN;
        case PL_BLUE:   return GL_BLUE;
        case PL_ALPHA:  return GL_ALPHA;
        default:        return channel;
    }
}

void plSwizzleTexture(PLTexture *texture, PLint r, PLint g, PLint b, PLint a) {
    _PL_GRAPHICS_TRACK();

    if(!texture) {
        return;
    }

    plBindTexture(texture);

#if defined(PL_MODE_OPENGL_CORE)
    if(PL_GL_VERSION(3,3)) {
        GLint swizzle[] = {
                _plTranslateColourChannel(r),
                _plTranslateColourChannel(g),
                _plTranslateColourChannel(b),
                _plTranslateColourChannel(a)
        };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    } else {
        // todo, software implementation
    }
#elif defined(PL_MODE_SOFTWARE)
#endif
}

PLresult plUploadTextureImage(PLTexture *texture, const PLImage *upload) {
    _PL_GRAPHICS_TRACK();

    plBindTexture(texture);

    texture->width = upload->width;
    texture->height = upload->height;
    texture->format = upload->format;
    texture->size = upload->size;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

#if defined(PL_MODE_OPENGL)
    PLuint levels = upload->levels;
    if(!levels) {
        levels = 1;
    }

    PLuint format = _plTranslateTextureFormat(upload->format);
    for(PLuint i = 0; i < levels; i++) {
        if (_plIsCompressedTextureFormat(upload->format)) {
            glCompressedTexSubImage2D
                    (
                            GL_TEXTURE_2D,
                            i,
                            upload->x, upload->y,
                            upload->width / (PLuint)pow(2, i),
                            upload->height / (PLuint)pow(2, i),
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

    return PL_RESULT_SUCCESS;
}

#if 0
PLresult plUploadTextureData(PLTexture *texture, const PLTextureInfo *upload) {
    _PL_GRAPHICS_TRACK();

    _plSetActiveTexture(texture);

#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    PLuint storage = _plTranslateTextureStorageFormat(upload->storage_type);
    PLuint format = _plTranslateTextureFormat(upload->format);

    PLuint levels = upload->levels;
    if(!levels) {
        levels = 1;
    }

    if (upload->initial) {
        glTexStorage2D(GL_TEXTURE_2D, levels, format, upload->width, upload->height);
    }

    // Check the format, to see if we're getting a compressed
    // format type.
    if (_plIsCompressedTextureFormat(upload->format)) {
        glCompressedTexSubImage2D
                (
                        GL_TEXTURE_2D,
                        0,
                        upload->x, upload->y,
                        upload->width, upload->height,
                        format,
                        upload->size,
                        upload->data
                );
    } else {
        glTexSubImage2D
                (
                        GL_TEXTURE_2D,
                        0,
                        upload->x, upload->y,
                        upload->width, upload->height,
                        _plTranslateColourFormat(upload->pixel_format),
                        storage,
                        upload->data
                );
    }

    if (plIsGraphicsStateEnabled(PL_CAPABILITY_GENERATEMIPMAP) && (levels > 1)) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
#elif defined (VL_MODE_GLIDE)
#elif defined (VL_MODE_DIRECT3D)
#endif

    return PL_RESULT_SUCCESS;
}
#endif

PLTexture *plGetCurrentTexture(PLuint tmu) {
    _PL_GRAPHICS_TRACK();

    return _pl_graphics_textures.find(pl_graphics_state.tmu[tmu].current_texture)->second;
}

PLuint plGetCurrentTextureUnit(void) {
    _PL_GRAPHICS_TRACK();

    return pl_graphics_state.current_textureunit;
}

#if defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

PLuint plGetMaxTextureSize(void) {
    _PL_GRAPHICS_TRACK();
    if (pl_graphics_state.hw_maxtexturesize != 0)
        return pl_graphics_state.hw_maxtexturesize;

#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&pl_graphics_state.hw_maxtexturesize);
#elif defined (VL_MODE_GLIDE)
    grGet(GR_MAX_TEXTURE_SIZE, sizeof(pl_graphics_state.hw_maxtexturesize), &pl_graphics_state.hw_maxtexturesize);
#endif

    return pl_graphics_state.hw_maxtexturesize;
}

PLuint plGetMaxTextureUnits(void) {
    _PL_GRAPHICS_TRACK();
    if (pl_graphics_state.hw_maxtextureunits != 0)
        return pl_graphics_state.hw_maxtextureunits;

#ifdef PL_MODE_OPENGL
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&pl_graphics_state.hw_maxtextureunits);
#elif defined (VL_MODE_GLIDE)
    grGet(GR_NUM_TMU, sizeof(param), (FxI32*)graphics_state.hw_maxtextureunits);
#endif

    return pl_graphics_state.hw_maxtextureunits;
}

PLuint plGetMaxTextureAnistropy(void) {
    _PL_GRAPHICS_TRACK();
    if (pl_graphics_state.hw_maxtextureanistropy != 0)
        return pl_graphics_state.hw_maxtextureanistropy;

#if defined (PL_MODE_OPENGL)
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat*)&pl_graphics_state.hw_maxtextureanistropy);
#endif

    return pl_graphics_state.hw_maxtextureanistropy;
}

#if defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

void plBindTexture(PLTexture *texture) {
    _PL_GRAPHICS_TRACK();

    if (texture->id == pl_graphics_state.tmu[plGetCurrentTextureUnit()].current_texture) {
        return;
    }

#if defined (PL_MODE_OPENGL)
    glBindTexture(GL_TEXTURE_2D, texture->id);
#endif

    pl_graphics_state.tmu[plGetCurrentTextureUnit()].current_texture = texture->id;
}

void plSetTextureUnit(PLuint target) {
    _PL_GRAPHICS_TRACK();
    if (target == pl_graphics_state.current_textureunit)
        return;
    else if (target > plGetMaxTextureUnits()) {
        plGraphicsLog("Attempted to select a texture image unit beyond what's supported by your hardware! (%i)\n",
                      target);
        return;
    }

#if defined (PL_MODE_OPENGL)
    glActiveTexture(_plTranslateTextureUnit(target));
#endif

    pl_graphics_state.current_textureunit = target;
}

PLresult plSetTextureAnisotropy(PLTexture *texture, PLuint amount) {
    _PL_GRAPHICS_TRACK();

    plBindTexture(texture);

#if defined (PL_MODE_OPENGL)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (PLint) amount);
#endif

    return PL_RESULT_SUCCESS;
}

PLresult plSetTextureFilter(PLTexture *texture, PLTextureFilter filter) {
    _PL_GRAPHICS_TRACK();

    plBindTexture(texture);

#ifdef PL_MODE_OPENGL
    PLuint filtermin, filtermax;
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

    return PL_RESULT_SUCCESS;
}

void plSetTextureEnvironmentMode(PLTextureEnvironmentMode mode) {
    _PL_GRAPHICS_TRACK();

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

/*===========================
	LIGHTING
===========================*/

PLLight *plCreateLight(void) {
    PLLight *light = (PLLight*)calloc(sizeof(PLLight), 1);
    if(!light) {
        plGraphicsLog("Failed to create light!\n");
        return NULL;
    }

    memset(light, 0, sizeof(PLLight));

    pl_graphics_state.num_lights++;
    light->colour = plCreateColour4b(255, 255, 255, 255);
    light->radius = 128.f;
    light->type = PLLIGHT_TYPE_OMNI;

    return light;
}

void plDeleteLight(PLLight *light) {
    if(!light) {
        return;
    }

    pl_graphics_state.num_lights--;
    free(light);
}

/*===========================
	UTILITY FUNCTIONS
===========================*/

#if 0
void plScreenshot(PLViewport *viewport, const PLchar *path) {
    PLbyte *buffer = (PLbyte*)calloc(viewport->height * viewport->width * 3, sizeof(PLbyte));
    glReadPixels(viewport->x, viewport->y, viewport->width, viewport->height, GL_RGB, GL_UNSIGNED_BYTE, buffer);

    free(buffer);
}
#endif

void plSetDefaultGraphicsState(void) {
    plSetClearColour(plCreateColour4b(PL_COLOUR_BLACK));
    plSetCullMode(PL_CULL_NEGATIVE);
    plSetTextureUnit(0);

#if defined(PL_MODE_OPENGL)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#if !defined(GL_VERSION_3_0)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif

    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);
#endif

    plEnableGraphicsStates(PL_CAPABILITY_SCISSORTEST);
}

void plFinish(void) {
    _PL_GRAPHICS_TRACK();

#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    glFinish();
#elif defined (VL_MODE_GLIDE)
    grFinish();
#elif defined (VL_MODE_DIRECT3D)
    // Not supported, or rather, we don't need this.
#endif
}

/////////////////////////////////////////////////////////////////////////////////////
// VIEWPORT/CAMERA

void plViewport(PLint x, PLint y, PLuint width, PLuint height) {
    _PL_GRAPHICS_TRACK();

    if (((x == pl_graphics_state.viewport_x) &&
         (y == pl_graphics_state.viewport_y)) &&
        ((width == pl_graphics_state.viewport_width) &&
         (height == pl_graphics_state.viewport_height)))
        return;

#if defined (PL_MODE_OPENGL)
    glViewport(x, y, width, height);
#elif defined (VL_MODE_DIRECT3D)
    D3D11_VIEWPORT viewport;
    memset(&viewport, 0, sizeof(D3D11_VIEWPORT));

    pl_graphics_state.viewport_x = viewport.TopLeftX = x;
    pl_graphics_state.viewport_y = viewport.TopLeftY = y;
    pl_graphics_state.viewport_width = viewport.Width = width;
    pl_graphics_state.viewport_height = viewport.Height = height;

    vl_d3d_context->lpVtbl->RSSetViewports(vl_d3d_context, 1, &viewport);
#endif

    pl_graphics_state.viewport_x        = x;
    pl_graphics_state.viewport_y        = y;
    pl_graphics_state.viewport_width    = width;
    pl_graphics_state.viewport_height   = height;
}

void plScissor(PLint x, PLint y, PLuint width, PLuint height) {
    _PL_GRAPHICS_TRACK();

#if defined (PL_MODE_OPENGL)
    glScissor(x, y, width, height);
#elif defined (VL_MODE_DIRECT3D)
    D3D11_RECT scissor_region;
    memset(&scissor_region, 0, sizeof(D3D11_RECT));
    scissor_region.bottom	= height;
    scissor_region.right	= width;
    vl_d3d_context->lpVtbl->RSSetScissorRects(vl_d3d_context, 0, &scissor_region);
#endif
}

// http://nehe.gamedev.net/article/replacement_for_gluperspective/21002/
void plPerspective(double fov_y, double aspect, double near, double far) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL)
    double h = tan(fov_y / 360 * PL_PI) * near;
    double w = h * aspect;
    glFrustum(-w, w, -h, h, near, far);
#endif
}
