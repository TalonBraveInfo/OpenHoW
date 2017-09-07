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

#include "PL/platform_model.h"

using namespace pl::graphics;

/*	PLATFORM MODEL LOADER	*/

PLVector3D plGenerateVertexNormal(PLVector3D a, PLVector3D b, PLVector3D c) {
#if 0
    PLVector3D x = c - b;
    PLVector3D y = a - b;
    return x.CrossProduct(y).Normalize();
#else
    return plNormalizeVector3D(plVector3DCrossProduct(
            plCreateVector3D(c.x - b.x, c.y - b.y, c.z - b.z), plCreateVector3D(a.x - b.x, a.y - b.y, a.z - b.z)));
#endif
}

void plGenerateStaticModelNormals(PLStaticModel *model) {
    PLModelFrame *frame = &model->frame;
#if 0 // per face...
    for (int i = 0; i < model->num_triangles; i++)
    {
        std::vector<float> normal = plGenerateNormal(
            frame->vertices[frame->triangles[i].indices[0]].position,
            frame->vertices[frame->triangles[i].indices[1]].position,
            frame->vertices[frame->triangles[i].indices[2]].position);

        frame->triangles[i].normal[PL_X] = normal[PL_X];
        frame->triangles[i].normal[PL_Y] = normal[PL_Y];
        frame->triangles[i].normal[PL_Z] = normal[PL_Z];
    }
#else // per vertex...
    for (PLVertex *vertex = &frame->vertices[0]; vertex; ++vertex) {
        for (PLTriangle *triangle = &frame->triangles[0]; triangle; ++triangle) {

        }
    }
#endif
}

void plGenerateAnimatedModelNormals(PLAnimatedModel *model) {
    if (!model)
        return;

    // Calculate normals for each frame... Can't we transform these? I guess that's
    // only feasible with skeletal animation formats where we can get the transform
    // but hell, if there's a way to abstractily grab the direction of a face then
    // surely we could figure that out.
    for (PLModelFrame *frame = &model->frames[0]; frame; ++frame) {
        for (PLVertex *vertex = &frame->vertices[0]; vertex; ++vertex) {
            for (PLTriangle *triangle = &frame->triangles[0]; triangle; ++triangle) {

            }
        }
    }
}

void plGenerateSkeletalModelNormals(PLSkeletalModel *model) {
    if (!model)
        return;
}

/*	Static Model    */

PLStaticModel *plCreateStaticModel(void) {
    _plSetCurrentFunction("plCreateStaticModel");

    PLStaticModel *model = new PLStaticModel;
    memset(model, 0, sizeof(PLStaticModel));

    return model;
}

// Less direct implementation to load a model (less efficient too).
PLStaticModel *plLoadStaticModel(const char *path) {
    if (!path || path[0] == ' ')
        return nullptr;

    std::string extension(path);
    if (extension.find(".pskx") != std::string::npos)
        return plLoadOBJModel(path);
    else if (extension.find(".obj") != std::string::npos)
        return plLoadOBJModel(path);

    return nullptr;
}

void plDeleteStaticModel(PLStaticModel *model) {
    if (!model) {
        _plSetErrorMessage("Invalid model!\n");
        return;
    }

    if (model->frame.triangles)
        delete model->frame.triangles;
    if (model->frame.vertices)
        delete model->frame.vertices;

    delete model;
}

/*	
	Animated Model
*/

PLAnimatedModel *plCreateAnimatedModel(void) {
    _plSetCurrentFunction("plCreateAnimatedModel");

    PLAnimatedModel *model = new PLAnimatedModel;
    memset(model, 0, sizeof(PLAnimatedModel));

    return model;
}

// Less direct implementation to load a model (less efficient too).
PLAnimatedModel *plLoadAnimatedModel(const char *path) {
    _plSetCurrentFunction("plLoadAnimatedModel");

    if (!path || path[0] == ' ') {
        _plSetErrorMessage("Invalid path!\n");
        return nullptr;
    }

    return nullptr;
}

void plDeleteAnimatedModel(PLAnimatedModel *model) {
    _plSetCurrentFunction("plDeleteAnimatedModel");

    if (!model) {
        _plSetErrorMessage("Invalid model!\n");
        return;
    }

    for (unsigned int i = 0; i < model->num_frames; i++) {
        if (&model->frames[i]) {
            if (model->frames[i].triangles)
                delete model->frames[i].triangles;
            if (model->frames[i].vertices)
                delete model->frames[i].vertices;
            delete &model->frames[i];
        }
    }

    delete model;
}

/*
	UNREAL PSKX Static Model Format

	Model format introduced in Unreal Engine 2.0, sadly rather
	hard to dig up much information about it.
*/

#define PSKX_EXTENSION "pskx"

