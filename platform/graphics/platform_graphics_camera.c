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

#define PLCAMERA_DEFAULT_WIDTH      640
#define PLCAMERA_DEFAULT_HEIGHT     480
#define PLCAMERA_DEFAULT_BOUNDS     5
#define PLCAMERA_DEFAULT_FOV        90
#define PLCAMERA_DEFAULT_NEAR       0.1
#define PLCAMERA_DEFAULT_FAR        100000

#define PLCAMERA_BUFFER_COLOUR  0
#define PLCAMERA_BUFFER_DEPTH   1

void _plInitCameras(void) {
    pl_graphics_state.max_cameras   = 1024;
    pl_graphics_state.cameras       = (PLCamera**)malloc(sizeof(PLCamera) * pl_graphics_state.max_cameras);
    pl_graphics_state.num_cameras   = 0;
}

void _plShutdownCameras(void) {

}

/////////////////////////////////////////////////////////////////////////////////////

PLCamera *plCreateCamera(void) {
    PLCamera *camera = (PLCamera*)calloc(1, sizeof(PLCamera));
    if(!camera) {
        _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate memory for Camera, %d!\n", sizeof(PLCamera));
        return NULL;
    }

    memset(camera, 0, sizeof(PLCamera));
    camera->fov     = PLCAMERA_DEFAULT_FOV;
    camera->near    = PLCAMERA_DEFAULT_NEAR;
    camera->far     = PLCAMERA_DEFAULT_FAR;
    camera->mode    = PLCAMERA_MODE_PERSPECTIVE;

    /*  XY * * * * W
     *  *
     *  *
     *  *
     *  *
     *  H
     */
    camera->viewport.w          = PLCAMERA_DEFAULT_WIDTH;
    camera->viewport.h          = PLCAMERA_DEFAULT_HEIGHT;
    camera->viewport.r_width    = 0;
    camera->viewport.r_height   = 0;

#if defined(PL_MODE_OPENGL)
    glGenFramebuffers(1, &camera->viewport.gl_framebuffer[0]);
    glGenRenderbuffers(1, &camera->viewport.gl_renderbuffer[PLCAMERA_BUFFER_COLOUR]);
    glGenRenderbuffers(1, &camera->viewport.gl_renderbuffer[PLCAMERA_BUFFER_DEPTH]);
#endif

    camera->bounds.mins = plCreateVector3D(
            -PLCAMERA_DEFAULT_BOUNDS, -PLCAMERA_DEFAULT_BOUNDS, -PLCAMERA_DEFAULT_BOUNDS);
    camera->bounds.maxs = plCreateVector3D(
            PLCAMERA_DEFAULT_BOUNDS, PLCAMERA_DEFAULT_BOUNDS, PLCAMERA_DEFAULT_BOUNDS);

    return camera;
}

void plDeleteCamera(PLCamera *camera) {
    if(!camera) {
        return;
    }

#if defined(PL_MODE_OPENGL)
    glDeleteFramebuffers(4, camera->viewport.gl_framebuffer);
    glDeleteRenderbuffers(4, camera->viewport.gl_renderbuffer);
#endif

    if(camera->viewport.v_buffer != NULL) {
        free(camera->viewport.v_buffer);
        camera->viewport.v_buffer = NULL;
    }

    free(camera);
}

#define _plUseBufferScaling(a) \
    ((a)->viewport.r_width != 0 && (a)->viewport.r_height != 0) && \
    ((a)->viewport.r_width != (a)->viewport.w && (a)->viewport.r_height != (a)->viewport.h)

void plSetupCamera(PLCamera *camera) {
    plAssert(camera);

    if(_plUseBufferScaling(camera)) {

        if(
                camera->viewport.old_r_height != camera->viewport.r_height &&
                camera->viewport.old_r_width != camera->viewport.r_width) {
            if (camera->viewport.v_buffer) {
                free(camera->viewport.v_buffer);
            }
            camera->viewport.v_buffer = (uint8_t *) malloc(camera->viewport.r_width * camera->viewport.r_height * 4);

            camera->viewport.old_r_width = camera->viewport.r_width;
            camera->viewport.old_r_height = camera->viewport.r_height;

            glDeleteFramebuffers(1, &camera->viewport.gl_framebuffer[0]);
            glDeleteRenderbuffers(1, &camera->viewport.gl_renderbuffer[PLCAMERA_BUFFER_DEPTH]);
            glDeleteRenderbuffers(1, &camera->viewport.gl_renderbuffer[PLCAMERA_BUFFER_COLOUR]);
        }

#if defined(PL_MODE_OPENGL)

        // Colour
        glBindRenderbuffer(GL_RENDERBUFFER, camera->viewport.gl_renderbuffer[PLCAMERA_BUFFER_COLOUR]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB4, camera->viewport.r_width, camera->viewport.r_height);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, camera->viewport.gl_framebuffer[0]);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                                  camera->viewport.gl_renderbuffer[PLCAMERA_BUFFER_COLOUR]);

        // Depth
        glBindRenderbuffer(GL_RENDERBUFFER, camera->viewport.gl_renderbuffer[PLCAMERA_BUFFER_DEPTH]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, camera->viewport.r_width, camera->viewport.r_height);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                  camera->viewport.gl_renderbuffer[PLCAMERA_BUFFER_DEPTH]);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            //PRINT(glErrorStringREGAL(glGetError()));
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
#endif
    }

    plViewport(camera->viewport.x, camera->viewport.y, camera->viewport.w, camera->viewport.h);
    plScissor(camera->viewport.x, camera->viewport.y, camera->viewport.w, camera->viewport.h);

#if defined(PL_MODE_OPENGL)
    // todo, modernize start
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // modernize end

    switch(camera->mode) {
        case PLCAMERA_MODE_PERSPECTIVE: {
            plPerspective(camera->fov, camera->viewport.w / camera->viewport.h, 0.1, 100000);

            // todo, modernize start
            glRotatef(camera->angles.y, 1, 0, 0);
            glRotatef(camera->angles.x, 0, 1, 0);
            glRotatef(camera->angles.z, 0, 0, 1);
            glTranslatef(camera->position.x, camera->position.y, camera->position.z);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            // modernize end

            break;
        }
            
        case PLCAMERA_MODE_ORTHOGRAPHIC: {
            glOrtho(0, camera->viewport.w, camera->viewport.h, 0, 0, 1000);
            break;
        }
        
        case PLCAMERA_MODE_ISOMETRIC: {
            glOrtho(-camera->fov, camera->fov, -camera->fov, 5, -5, 40);

            // todo, modernize start
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glRotatef(35.264f, 1, 0, 0);
            glRotatef(camera->angles.x, 0, 1, 0);

            glTranslatef(camera->position.x, camera->position.y, camera->position.z);
            // modernize end
            break;
        }

        default: break;
    }
#endif
}

/////////////////////////////////////////////////////////////////////////////////////

void plDrawPerspective(void) {
    for(PLCamera **camera = pl_graphics_state.cameras;
        camera < pl_graphics_state.cameras + pl_graphics_state.num_cameras; ++camera) {
        plAssert(camera);

        plSetupCamera((*camera));

        // todo, draw stuff...

        if(_plUseBufferScaling((*camera))) {

            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(0, 0, (*camera)->viewport.r_width, (*camera)->viewport.r_height, GL_BGRA, GL_UNSIGNED_BYTE,
                         &(*camera)->viewport.v_buffer[0]);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            plScissor(0, 0, (*camera)->viewport.w, (*camera)->viewport.h);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, (*camera)->viewport.gl_framebuffer[0]);
            glBlitFramebuffer(
                    0, 0,
                    (*camera)->viewport.r_width, (*camera)->viewport.r_height,
                    0, 0,
                    (*camera)->viewport.w, (*camera)->viewport.h,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST
            );
        }
    }
}
