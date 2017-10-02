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

#include "model_private.h"

#include <PL/platform_filesystem.h>

/*	PLATFORM MODEL LOADER	*/

typedef struct PLModelInterface {
    const char *extension;

    PLModel*(*Load)(const char *path);
} PLModelInterface;

PLModelInterface model_interfaces[]= {
        { "mdl", _plLoadRequiemModel },
        //{ "vtx", _plLoadStaticHOWModel },
        //{ "mdl", _plLoadStaticSourceModel },
        //{ "mdl", _plLoadStaticGoldSrcModel },
        //{ "smd", _plLoadStaticSMDModel },
        //{ "obj", _plLoadOBJModel },
};

///////////////////////////////////////

void _plGenerateModelNormals(PLModel *model) {
    plAssert(model);
    for(unsigned int i = 0; i < model->num_meshes; ++i) {
        plGenerateMeshNormals(model->meshes[i]);
    }
}

void _plGenerateModelAABB(PLModel *model) {
    plAssert(model);
    for(unsigned int i = 0; i < model->num_meshes; ++i) {
        plAddAABB(&model->bounds, plCalculateMeshAABB(model->meshes[i]));
    }
}

void _plGenerateAnimatedModelNormals(PLAnimatedModel *model) {
    plAssert(model);
    for (PLModelFrame *frame = &model->frames[0]; frame; ++frame) {
        for(unsigned int i = 0; i < frame->num_meshes; ++i) {
            plGenerateMeshNormals(frame->meshes[i]);
        }
    }
}

///////////////////////////////////////

PLModel *plLoadModel(const char *path) {
    if(!plFileExists(path)) {
        _plReportError(PL_RESULT_FILEREAD, "Failed to load model, %s!", path);
        return NULL;
    }

    const char *extension = plGetFileExtension(path);
    for(unsigned int i = 0; i < plArrayElements(model_interfaces); ++i) {
        if(model_interfaces[i].Load == NULL) {
            continue;
        }

        if(model_interfaces[i].extension != '\0') {
            if (!strcmp(extension, model_interfaces[i].extension)) {
                PLModel *model = model_interfaces[i].Load(path);
                if(model != NULL) {
                    return model;
                }
            }
        }
    }

    return NULL;
}

void plDeleteModel(PLModel *model) {
    plAssert(model);
    for(unsigned int i = 0; i < model->num_meshes; ++i) {
        if(model->meshes[i] == NULL) {
            continue;
        }

        plDeleteMesh(model->meshes[i]);
    }

    free(model);
}

void plDrawModel(PLModel *model) {
    plAssert(model);
    for(unsigned int i = 0; i < model->num_meshes; ++i) {
        plDrawMesh(model->meshes[i]);
    }
}
