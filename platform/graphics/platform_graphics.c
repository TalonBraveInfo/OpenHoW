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

#include <PL/platform_log.h>
#include <PL/platform_image.h>
#include <PL/platform_console.h>

#include "graphics_private.h"

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

#if !defined(PL_MODE_OPENGL_CORE)
bool pl_gl_generate_mipmap              = false;
bool pl_gl_depth_texture                = false;
bool pl_gl_shadow                       = false;
bool pl_gl_vertex_buffer_object         = false;
bool pl_gl_texture_compression          = false;
bool pl_gl_texture_compression_s3tc     = false;
bool pl_gl_multitexture                 = false;
bool pl_gl_texture_env_combine          = false;
bool pl_gl_texture_env_add              = false;
bool pl_gl_vertex_program               = false;
bool pl_gl_fragment_program             = false;
#endif

unsigned int pl_gl_version_major = 0;
unsigned int pl_gl_version_minor = 0;

void _plInitOpenGL(void) {
    _PL_GRAPHICS_TRACK();

    // todo, write our own replacement for GLEW

#if 0
#if !defined(PL_MODE_OPENGL_CORE)
    PLuint err = glewInit();
    if (err != GLEW_OK) {
        plGraphicsLog("Failed to initialize GLEW!\n%s\n", glewGetErrorString(err));
    }

    // Check that the required capabilities are supported.
    if (GLEW_ARB_multitexture) pl_gl_multitexture = true;
    else plGraphicsLog("Video hardware incapable of multi-texturing!\n");
    if (GLEW_ARB_texture_env_combine || GLEW_EXT_texture_env_combine) pl_gl_texture_env_combine = true;
    else plGraphicsLog("ARB/EXT_texture_env_combine isn't supported by your hardware!\n");
    if (GLEW_ARB_texture_env_add || GLEW_EXT_texture_env_add) pl_gl_texture_env_add = true;
    else plGraphicsLog("ARB/EXT_texture_env_add isn't supported by your hardware!\n");
    if (GLEW_ARB_vertex_program || GLEW_ARB_fragment_program) {
        pl_gl_vertex_program = true;
        pl_gl_fragment_program = true;
    }
    else plGraphicsLog("Shaders aren't supported by this hardware!\n");
    if (GLEW_SGIS_generate_mipmap) pl_gl_generate_mipmap = true;
    else plGraphicsLog("Hardware mipmap generation isn't supported!\n");
    if (GLEW_ARB_depth_texture) pl_gl_depth_texture = true;
    else plGraphicsLog("ARB_depth_texture isn't supported by your hardware!\n");
    if (GLEW_ARB_shadow) pl_gl_shadow = true;
    else plGraphicsLog("ARB_shadow isn't supported by your hardware!\n");
    if (GLEW_ARB_vertex_buffer_object) pl_gl_vertex_buffer_object = true;
    else plGraphicsLog("Hardware doesn't support Vertex Buffer Objects!\n");

    // If HW compression isn't supported then we'll need to do
    // all of this in software later.
    if (GLEW_ARB_texture_compression) {
        if (GLEW_EXT_texture_compression_s3tc) {
            pl_gl_texture_compression_s3tc = true;
        }
    }
#endif
#endif

#if 0 // todo, move post window initialization... ?
    const char *version = _plGetHWVersion();
    pl_gl_version_major = (unsigned int) atoi(&version[0]);
    pl_gl_version_minor = (unsigned int) atoi(&version[2]);
#endif
}

void _plShutdownOpenGL() {}

#else

#define DEFAULT_BUFFER_WIDTH    320
#define DEFAULT_BUFFER_HEIGHT   240

uint8_t *pl_sw_backbuffer;
uint32_t pl_sw_backbuffer_size = DEFAULT_BUFFER_WIDTH * DEFAULT_BUFFER_HEIGHT * 4;
uint8_t *pl_sw_frontbuffer;

void _plInitSoftware(void) {
    if(!(pl_sw_backbuffer = (uint8_t*)malloc(pl_sw_backbuffer_size))) {
        _plReportError(PL_RESULT_MEMORYALLOC, "", NULL);
        return;
    }
}

void _plShutdownSoftware(void) {
}

#endif

void _plInitTextures(void); // platform_graphics_texture
void _plInitCameras(void);  // platform_graphics_camera

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
#else
    _plInitSoftware();
#endif

    _plInitCameras();
    _plInitTextures();

    // Get any information that will be presented later.
    pl_graphics_state.hw_extensions     = _plGetHWExtensions();
    pl_graphics_state.hw_renderer       = _plGetHWRenderer();
    pl_graphics_state.hw_vendor         = _plGetHWVendor();
    pl_graphics_state.hw_version        = _plGetHWVersion();
    plGraphicsLog(" HARDWARE/DRIVER INFORMATION\n");
    plGraphicsLog("  RENDERER: %s\n", pl_graphics_state.hw_renderer);
    plGraphicsLog("  VENDOR:   %s\n", pl_graphics_state.hw_vendor);
    plGraphicsLog("  VERSION:  %s\n\n", pl_graphics_state.hw_version);

    return PL_RESULT_SUCCESS;
}

void _plShutdownTextures(void); // platform_graphics_texture
void _plShutdownCameras(void);  // platform_graphics_camera

void _plShutdownGraphics(void) {
    _PL_GRAPHICS_TRACK();

#if defined (PL_MODE_OPENGL)
#elif defined (VL_MODE_GLIDE)
#elif defined (VL_MODE_DIRECT3D)
    _plShutdownDirect3D();
#elif defined (PL_MODE_SOFTWARE)
    _plShutdownSoftware();
#endif

    _plShutdownCameras();
    _plShutdownTextures();
}

/*===========================
	HARDWARE INFORMATION
===========================*/

const PLchar *_plGetHWExtensions(void) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL)
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

#if defined(PL_MODE_OPENGL)
    return (const PLchar *) glGetString(GL_RENDERER);
#elif defined (VL_MODE_GLIDE)
    return grGetString(GR_RENDERER);
#else
    return "";
#endif
}

const PLchar *_plGetHWVendor(void) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL)
    return (const PLchar *) glGetString(GL_VENDOR);
#elif defined (VL_MODE_GLIDE)
    return grGetString(GR_VENDOR);
#else
    return "";
#endif
}

const char *_plGetHWVersion(void) {
    _PL_GRAPHICS_TRACK();

#if defined(PL_MODE_OPENGL)
    return (const char *) glGetString(GL_VERSION);
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

PLFrameBuffer *plCreateFrameBuffer(unsigned int w, unsigned int h) {
    PLFrameBuffer *buffer = (PLFrameBuffer*)malloc(sizeof(PLFrameBuffer));
    if(!buffer) {
        _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate %d bytes for FrameBuffer!", sizeof(PLFrameBuffer));
        return NULL;
    }

#if defined(PL_MODE_OPENGL)
    glGenFramebuffers(1, &buffer->fbo);
    glGenRenderbuffers(1, &buffer->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, w, h);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer->fbo);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, buffer->rbo);
#endif

    return buffer;
}

void plDeleteFrameBuffer(PLFrameBuffer *buffer) {
    if(!buffer) {
        return;
    }

#if defined(PL_MODE_OPENGL)
    glDeleteFramebuffers(1, &buffer->fbo);
    glDeleteRenderbuffers(1, &buffer->rbo);
#endif
}

void plBindFrameBuffer(PLFrameBuffer *buffer) {
    if(!buffer) {
        // todo, warning regarding invalid buffer blah blah
        return;
    }

#if defined(PL_MODE_OPENGL)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer->fbo);
#endif
}

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

void plClearBuffers(unsigned int buffers) {
    _PL_GRAPHICS_TRACK();

#if defined (PL_MODE_OPENGL) || defined (PL_MODE_OPENGL_CORE)
    // Rather ugly, but translate it over to GL.
    unsigned int glclear = 0;
    if(buffers & PL_BUFFER_COLOUR)  glclear |= GL_COLOR_BUFFER_BIT;
    if(buffers & PL_BUFFER_DEPTH)   glclear |= GL_DEPTH_BUFFER_BIT;
    if(buffers & PL_BUFFER_STENCIL) glclear |= GL_STENCIL_BUFFER_BIT;
    glClear(glclear);
#elif defined (PL_MODE_GLIDE)
    // Glide only supports clearing a single buffer.
    grBufferClear(
        // Convert buffer_clearcolour to something that works with Glide.
        _plConvertColour4fv(PL_COLOURFORMAT_RGBA, graphics_state.buffer_clearcolour),
        1, 1);
#elif defined (PL_MODE_DIRECT3D)
    pl_d3d_context->lpVtbl->ClearRenderTargetView(pl_d3d_context,
        pl_d3d_backbuffer,
        graphics_state.current_clearcolour
    );
#elif defined (PL_MODE_SOFTWARE)
    if(buffers & PL_BUFFER_COLOUR) {
        for (unsigned int i = 0; i < pl_sw_backbuffer_size; i += 4) {
            pl_sw_backbuffer[i] = pl_graphics_state.current_clearcolour.r;
            pl_sw_backbuffer[i + 1] = pl_graphics_state.current_clearcolour.g;
            pl_sw_backbuffer[i + 2] = pl_graphics_state.current_clearcolour.b;
            pl_sw_backbuffer[i + 3] = pl_graphics_state.current_clearcolour.a;
        }
    }
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
        { PL_CAPABILITY_BLEND, 0, "BLEND" },
        { PL_CAPABILITY_DEPTHTEST, 0, "DEPTH_TEST" },
        { PL_CAPABILITY_TEXTURE_2D, 0, "TEXTURE_2D" },
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

    for (int i = 0; i < sizeof(graphics_capabilities); i++) {
        if (graphics_capabilities[i].pl_parm == 0) break;

        if (pl_graphics_state.mode_debug) {
            plGraphicsLog("Enabling %s\n", graphics_capabilities[i].ident);
        }

        if (flags & PL_CAPABILITY_TEXTURE_2D) {
            pl_graphics_state.tmu[pl_graphics_state.current_textureunit].active = true;
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

    for (int i = 0; i < sizeof(graphics_capabilities); i++) {
        if (graphics_capabilities[i].pl_parm == 0) break;

        if (pl_graphics_state.mode_debug) {
            plGraphicsLog("Disabling %s\n", graphics_capabilities[i].ident);
        }

        if (flags & PL_CAPABILITY_TEXTURE_2D) {
            pl_graphics_state.tmu[pl_graphics_state.current_textureunit].active = false;
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

#if defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

#if defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

/*===========================
	UTILITY FUNCTIONS
===========================*/

#if 0
void plScreenshot(PLViewport *viewport, const PLchar *path) {
    uint8_t *buffer = (uint8_t*)calloc(viewport->height * viewport->width * 3, sizeof(uint8_t));
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

void plViewport(int x, int y, unsigned int width, unsigned int height) {
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

void plScissor(int x, int y, unsigned int width, unsigned int height) {
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

/* DRAWING  */

void plDrawPixel(int x, int y, PLColour colour) {
}

/////////////////////////////////////////////////////////////////////////////////////

void plProcessGraphics(void) {
    plClearBuffers(PL_BUFFER_COLOUR | PL_BUFFER_DEPTH | PL_BUFFER_STENCIL);

    plDrawConsole();
    plDrawPerspective();
}