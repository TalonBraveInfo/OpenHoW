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

#define PLCAMERA_DEFAULT_WIDTH      640
#define PLCAMERA_DEFAULT_HEIGHT     480
#define PLCAMERA_DEFAULT_BOUNDS     5
#define PLCAMERA_DEFAULT_FOV        90
#define PLCAMERA_DEFAULT_NEAR       0.1
#define PLCAMERA_DEFAULT_FAR        100000

PLCamera *plCreateCamera(void) {
    PLCamera *camera = (PLCamera*)calloc(1, sizeof(PLCamera));
    if(!camera) {
        plSetError("Failed to allocate camera object!\n");
        return NULL;
    }

    memset(camera, 0, sizeof(PLCamera));
    camera->fov     = PLCAMERA_DEFAULT_FOV;
    camera->near    = PLCAMERA_DEFAULT_NEAR;
    camera->far     = PLCAMERA_DEFAULT_FAR;
    camera->mode    = PL_CAMERAMODE_PERSPECTIVE;

    /*  XY * * * * W
     *  *
     *  *
     *  *
     *  *
     *  H
     */
    camera->viewport.width   = PLCAMERA_DEFAULT_WIDTH;
    camera->viewport.height  = PLCAMERA_DEFAULT_HEIGHT;

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

    free(camera);
}

// http://nehe.gamedev.net/article/replacement_for_gluperspective/21002/
void _plPerspective(double fov_y, double aspect, double near, double far) {
    double h = tan(fov_y / 360 * PL_PI) * near;
    double w = h * aspect;
    glFrustum(-w, w, -h, h, near, far);
}

void plSetupCamera(PLCamera *camera) {
    if(!camera) {
        return;
    }

    plViewport(camera->viewport.x, camera->viewport.y, camera->viewport.width, camera->viewport.height);
    plScissor(camera->viewport.x, camera->viewport.y, camera->viewport.width, camera->viewport.height);

    // todo, modernize...

//#if defined(PL_MODE_OPENGL_CORE)

//#elif defined(PL_MODE_OPENGL)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    switch(camera->mode) {
        default:
        case PL_CAMERAMODE_PERSPECTIVE: {
            _plPerspective(camera->fov, camera->viewport.width / camera->viewport.height, 0.1, 100000);

            // todo, update start
            //gluPerspective(camera->fov, camera->viewport.width / camera->viewport.height, 0.1f, 100000.f);

            glRotatef(camera->angles.y, 1, 0, 0);
            glRotatef(camera->angles.x, 0, 1, 0);
            glRotatef(camera->angles.z, 0, 0, 1);
            glTranslatef(camera->position.x, camera->position.y, camera->position.z);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            // todo, update end

            break;
        }
        case PL_CAMERAMODE_ORTHOGRAPHIC: {
            glOrtho(0, camera->viewport.width, camera->viewport.height, 0, 0, 1000);
            break;
        }
        case PL_CAMERAMODE_ISOMETRIC: {
            glOrtho(-camera->fov, camera->fov, -camera->fov, 5, -5, 40);

            // todo, update start
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glRotatef(35.264f, 1, 0, 0);
            glRotatef(camera->angles.x, 0, 1, 0);

            glTranslatef(camera->position.x, camera->position.y, camera->position.z);
            // todo, update end
            break;
        }
    }
//#endif
}
