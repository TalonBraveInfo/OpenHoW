/*
Copyright (C) 2011-2016 OldTimes Software

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "PL/platform.h"

#include "PL/platform_model.h"

using namespace pl::graphics;

/*
	UNREAL 3D Animated Model Format

	The following is based on information from the following page...
	http://paulbourke.net/dataformats/unreal/
*/

#define    U3D_FILE_EXTENSION "3d"

typedef struct U3DAnimationHeader_s {
    PLuint16 frames;    // Number of frames.
    PLuint16 size;    // Size of each frame.
} U3DAnimationHeader;

typedef struct U3DDataHeader_s {
    PLuint16 numpolys;    // Number of polygons.
    PLuint16 numverts;    // Number of vertices.
    PLuint16 rotation;    // Mesh rotation?
    PLuint16 frame;        // Initial frame.

    PLuint32 norm_x;
    PLuint32 norm_y;
    PLuint32 norm_z;

    PLuint32 fixscale;
    PLuint32 unused[3];
} U3DDataHeader;

#define    U3D_FLAG_UNLIT       16
#define    U3D_FLAG_FLAT        32
#define    U3D_FLAG_ENVIRONMENT 64
#define    U3D_FLAG_NEAREST     128

enum U3DType {
    U3D_TYPE_NORMAL,
    U3D_TYPE_NORMALTWOSIDED,
    U3D_TYPE_TRANSLUCENT,
    U3D_TYPE_MASKED,
    U3D_TYPE_MODULATE,
    U3D_TYPE_ATTACHMENT
};

typedef struct U3DVertex_s {
    // This is a bit funky...
    PLint32 x : 11;
    PLint32 y : 11;
    PLint32 z : 10;
} U3DVertex;

typedef struct U3DTriangle_s {
    PLuint16 vertex[3]; // Vertex indices

    PLuint8 type;       // Triangle type
    PLuint8 colour;     // Triangle colour
    PLuint8 ST[3][2];   // Texture coords
    PLuint8 texturenum; // Texture offset
    PLuint8 flags;      // Triangle flags
} U3DTriangle;

FILE *pl_u3d_dataf = nullptr;
FILE *pl_u3d_animf = nullptr;

void _plUnloadU3DModel() {
    if (pl_u3d_animf) {
        fclose(pl_u3d_animf);
    }
    if (pl_u3d_dataf) {
        fclose(pl_u3d_dataf);
    }
}

PLAnimatedModel *plLoadU3DModel(const PLchar *path) {
    plSetErrorFunction("plLoadU3DModel");

    pl_u3d_dataf = std::fopen(path, "rb");
    if (!pl_u3d_dataf) {
        plSetError("Failed to load data file! (%s)\n", path);
        return NULL;
    }

    // Try to figure out the data string we used
    // then erase it.
    std::string newpath(path);
    size_t strpos = newpath.find("_d.3d");
    if (strpos == std::string::npos)
        // Some legacy models use _Data...
        strpos = newpath.find("_Data.3d");

    if (strpos != std::string::npos)
        newpath.erase(strpos);
    else {
        plSetError("Invalid file name! (%s)\n", newpath.c_str());

        _plUnloadU3DModel();
        return nullptr;
    }
    newpath.append("_a.3d");

    // Attempt to load the animation file.
    pl_u3d_animf = fopen(newpath.c_str(), "rb");
    if (!pl_u3d_animf) {
        // Some legacy models use _Anim...
        newpath.erase(newpath.length() - 5);
        newpath.append("_Anim.3d");

        pl_u3d_animf = fopen(newpath.c_str(), "r");
        if (!pl_u3d_animf) {
            plSetError("Failed to load U3D animation data! (%s)\n", newpath.c_str());

            _plUnloadU3DModel();
            return nullptr;
        }
    }

    // Attempt to read the animation header.
    U3DAnimationHeader animheader;
    if (fread(&animheader, sizeof(U3DAnimationHeader), 1, pl_u3d_animf) != 1) {
        plSetError("Failed to read animation file!\n");

        _plUnloadU3DModel();
        return nullptr;
    }

    // Attempt to read the data header.
    U3DDataHeader dataheader;
    if (fread(&dataheader, sizeof(U3DDataHeader), 1, pl_u3d_dataf) != 1) {
        plSetError("Failed to read data file!\n");

        _plUnloadU3DModel();
        return nullptr;
    }

    PLAnimatedModel *model = plCreateAnimatedModel();
    if (!model) {
        plSetError("Failed to allocate animated model!\n");

        _plUnloadU3DModel();
        return NULL;
    }

    // Store the information we've gathered.
    model->num_frames = animheader.frames;
    model->num_triangles = dataheader.numpolys;
    model->num_vertices = dataheader.numverts;

    // Allocate the triangle/vertex arrays.
    model->frames = new PLModelFrame[model->num_frames];
    for (PLuint i = 0; i < model->num_frames; i++) {
        model->frames[i].vertices = new PLVertex[model->num_vertices];
        model->frames[i].triangles = new PLTriangle[model->num_triangles];
    }

    // Skip unused header data.
    std::fseek(pl_u3d_dataf, 12, SEEK_CUR);

    // Go through each triangle.
    std::vector<U3DTriangle> utriangles;
    for (unsigned int i = 0; i < model->num_triangles; i++) {
        if (std::fread(&utriangles[i], sizeof(U3DTriangle), 1, pl_u3d_dataf) != 1) {
            plSetError("Failed to process triangles! (%i)\n", i);

            plDeleteAnimatedModel(model);

            _plUnloadU3DModel();
            return nullptr;
        }

        // Copy the indices over.
        model->frames[0].triangles[i].indices[0] = utriangles[i].vertex[0];
        model->frames[0].triangles[i].indices[1] = utriangles[i].vertex[1];
        model->frames[0].triangles[i].indices[2] = utriangles[i].vertex[2];
    }

    // Go through each vertex.
    std::vector<U3DVertex> uvertices;
    for (unsigned int i = 0; i < model->num_frames; i++) {
        if (std::fread(&uvertices[i], sizeof(U3DVertex), 1, pl_u3d_animf) != 1) {
            plSetError("Failed to process vertex! (%i)\n", i);

            plDeleteAnimatedModel(model);

            _plUnloadU3DModel();
            return nullptr;
        }

        for (unsigned int j = 0; j < model->num_triangles; j++) {

        }
    }

    // Calculate normals.
    plGenerateAnimatedModelNormals(model);

    _plUnloadU3DModel();

    return model;
}
