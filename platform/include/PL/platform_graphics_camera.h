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

#include "platform.h"

enum {
    PLCAMERA_MODE_PERSPECTIVE,
    PLCAMERA_MODE_ORTHOGRAPHIC,
    PLCAMERA_MODE_ISOMETRIC
};

typedef struct PLWindow PLWindow;

typedef struct PLCamera {
    double fov;
    double near, far;
    unsigned int mode;

    PLVector3D angles, position;
    PLBBox3D bounds;

    // Viewport
    PLRectangle viewport;
    uint8_t *v_buffer;
#if defined(PL_MODE_OPENGL)
    unsigned int gl_framebuffer[4];
    unsigned int gl_renderbuffer[4];
#endif

    unsigned int r_width, r_height;
    unsigned int old_r_width, old_r_height;
} PLCamera;

PL_EXTERN_C

PL_EXTERN PLCamera *plCreateCamera(void);
PL_EXTERN void plDeleteCamera(PLCamera *camera);

PL_EXTERN void plSetupCamera(PLCamera *camera);

PL_EXTERN void plDrawPerspective(void);

PL_EXTERN_C_END
