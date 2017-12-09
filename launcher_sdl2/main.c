/* OpenHOW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "main.h"

#include "formats/mad.h"
//#include "formats/pog.h"

#include "font.h"
#include "object.h"
#include "map.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <GLFW/glfw3.h>

#define WIDTH 1024
#define HEIGHT 768

/*  SRL Format Specification        */
/* The SRL format is used as an index for sounds used
 * by the game. It's just a text-based format, nothing
 * particular special.
 */

void load_srl_file(const char *path) {
    // todo, parse srl file format
    // 099 < number of sounds in the document
    // 000 < ???
    // 000 < ???
}

//////////////////////////////////////////////////////

void load_fac_file(const char *path) {
    FACHeader header;
    memset(&header, 0, sizeof(FACHeader));

    PRINT("\nOpening %s\n", path);

    FILE *file = fopen(path, "r");
    if(!file) {
        PRINT_ERROR("Failed to load file %s!\n", path);
    }

    if(fread(&header, sizeof(FACHeader), 1, file) != 1) {
        PRINT_ERROR("Invalid file header...\n");
    }
#if 0
    for(int i = 0; i < plArrayElements(header.unknown0); i++) {
        if(header.unknown0[0] != '\0') {
            PRINT_ERROR("Invalid FAC file!\n");
        }
    }
#endif

    PRINT("triangles: %d\n", header.num_triangles);
    FACTriangle triangles[header.num_triangles];
    if(header.num_triangles != 0) {
        if(fread(triangles, sizeof(FACTriangle), header.num_triangles, file) != header.num_triangles) {
            PRINT_ERROR("Unexpected block size!\n");
        }

        int texture_table[32];
        memset(texture_table, -1, sizeof(int) * 32);

#if 1
        for(unsigned int i = 0; i < header.num_triangles; i++) {
            PRINT("TRIANGLE %d\n", i);
            PRINT("    indices(%d %d %d)\n", triangles[i].indices[0], triangles[i].indices[1], triangles[i].indices[2]);
            PRINT("    normals(%d %d %d)\n", triangles[i].normal[0], triangles[i].normal[1], triangles[i].normal[2]);
            PRINT("    texture index(%d)\n", triangles[i].texture_index);
            //PRINT("    texture coords(%d %d)\n", triangles[i].texture_coords[0], triangles[i].texture_coords[1]);

            for(unsigned int j = 0; j < plArrayElements(texture_table); j++) {
                if(texture_table[j] == triangles[i].texture_index) {
                    continue;
                } else if(texture_table[j] == -1) {
                    texture_table[j] = triangles[i].texture_index;
                }
            }
        }

        for(unsigned int j = 0; j < plArrayElements(texture_table); j++) {
            PRINT("%d\n", texture_table[j]);
        }
#endif
    }

    FACQuad quads[2048];
    memset(quads, 0, sizeof(FACQuad));
    unsigned int num_quads = (unsigned int) fread(quads, sizeof(FACQuad), 2048, file);
    PRINT("\nquads: %d\n\n", model.num_triangles);
#if 0
    for(unsigned int i = 0; i < num_quads; i++) {
        PRINT("QUAD %d\n", i);
        PRINT("    indices(%d %d %d %d)\n",
              quads[i].indices[0], quads[i].indices[1], quads[i].indices[2], quads[i].indices[3]);
        PRINT("    normals(%d %d %d %d)\n",
              quads[i].normal[0], quads[i].normal[1], quads[i].normal[2], quads[i].normal[3]);
        PRINT("    texture index(%d)\n", quads[i].texture_index);
    }
#endif

    model.num_triangles = header.num_triangles + (num_quads * 2);
    model.num_vertices = model.num_triangles * 3;

    model.tri_mesh = plCreateMesh(
            PLMESH_TRIANGLES,
            PL_DRAW_IMMEDIATE,
            model.num_triangles,
            model.num_vertices
    );
    unsigned int cur_vert = 0; uint8_t r, g, b;
    PLVector3D normal;
    for(unsigned int i = 0; i < header.num_triangles; i++, cur_vert++) {

        PLVector3D va = plCreateVector3D(model.coords[triangles[i].indices[0]].x,
                                         model.coords[triangles[i].indices[0]].y,
                                         model.coords[triangles[i].indices[0]].z);
        PLVector3D vb = plCreateVector3D(model.coords[triangles[i].indices[1]].x,
                                         model.coords[triangles[i].indices[1]].y,
                                         model.coords[triangles[i].indices[1]].z);
        PLVector3D vc = plCreateVector3D(model.coords[triangles[i].indices[2]].x,
                                         model.coords[triangles[i].indices[2]].y,
                                         model.coords[triangles[i].indices[2]].z);
        normal = plGenerateVertexNormal(va, vb, vc);

        plSetMeshVertexPosition3f(model.tri_mesh, cur_vert,
                                  model.coords[triangles[i].indices[0]].x +
                                  model.bones[model.coords[triangles[i].indices[0]].bone_index].coords[0],
                                  model.coords[triangles[i].indices[0]].y +
                                  model.bones[model.coords[triangles[i].indices[0]].bone_index].coords[1],
                                  model.coords[triangles[i].indices[0]].z +
                                  model.bones[model.coords[triangles[i].indices[0]].bone_index].coords[2]
        );
        plSetMeshVertexNormal3f(model.tri_mesh, cur_vert, normal.x, normal.y, normal.z);
        srand(model.coords[triangles[i].indices[0]].bone_index);
        r = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        g = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        b = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        plSetMeshVertexColour(model.tri_mesh, cur_vert, plCreateColour4b(r, g, b, 255));
        cur_vert++;

        plSetMeshVertexPosition3f(model.tri_mesh, cur_vert,
                                  model.coords[triangles[i].indices[1]].x +
                                  model.bones[model.coords[triangles[i].indices[1]].bone_index].coords[0],
                                  model.coords[triangles[i].indices[1]].y +
                                  model.bones[model.coords[triangles[i].indices[1]].bone_index].coords[1],
                                  model.coords[triangles[i].indices[1]].z +
                                  model.bones[model.coords[triangles[i].indices[1]].bone_index].coords[2]
        );
        plSetMeshVertexNormal3f(model.tri_mesh, cur_vert, normal.x, normal.y, normal.z);
        srand(model.coords[triangles[i].indices[1]].bone_index);
        r = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        g = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        b = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        plSetMeshVertexColour(model.tri_mesh, cur_vert, plCreateColour4b(r, g, b, 255));
        cur_vert++;

        plSetMeshVertexPosition3f(model.tri_mesh, cur_vert,
                                  model.coords[triangles[i].indices[2]].x +
                                  model.bones[model.coords[triangles[i].indices[2]].bone_index].coords[0],
                                  model.coords[triangles[i].indices[2]].y +
                                  model.bones[model.coords[triangles[i].indices[2]].bone_index].coords[1],
                                  model.coords[triangles[i].indices[2]].z +
                                  model.bones[model.coords[triangles[i].indices[2]].bone_index].coords[2]
        );
        plSetMeshVertexNormal3f(model.tri_mesh, cur_vert, normal.x, normal.y, normal.z);
        srand(model.coords[triangles[i].indices[2]].bone_index);
        r = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        g = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        b = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        plSetMeshVertexColour(model.tri_mesh, cur_vert, plCreateColour4b(r, g, b, 255));

#if 0
        PRINT(" %d 0(%d %d %d) 1(%d %d %d) 2(%d %d %d)\n",
              i,

              model.coords[triangles[i].indices[0]].x,
              model.coords[triangles[i].indices[0]].y,
              model.coords[triangles[i].indices[0]].z,

              model.coords[triangles[i].indices[1]].x,
              model.coords[triangles[i].indices[1]].y,
              model.coords[triangles[i].indices[1]].z,

              model.coords[triangles[i].indices[2]].x,
              model.coords[triangles[i].indices[2]].y,
              model.coords[triangles[i].indices[2]].z
        );
#endif
    }

#if 1
    for(unsigned int i = 0; i < num_quads; i++, cur_vert++) {

        PLVector3D va = plCreateVector3D(model.coords[quads[i].normal[0]].x,
                                        model.coords[quads[i].normal[0]].y,
                                        model.coords[quads[i].normal[0]].z);
        PLVector3D vb = plCreateVector3D(model.coords[quads[i].normal[1]].x,
                                        model.coords[quads[i].normal[1]].y,
                                        model.coords[quads[i].normal[1]].z);
        PLVector3D vc = plCreateVector3D(model.coords[quads[i].normal[2]].x,
                                        model.coords[quads[i].normal[2]].y,
                                        model.coords[quads[i].normal[2]].z);
        normal = plGenerateVertexNormal(va, vb, vc);

        plSetMeshVertexPosition3f(model.tri_mesh, cur_vert,
                                  model.coords[quads[i].indices[0]].x +
                                  model.bones[model.coords[quads[i].indices[0]].bone_index].coords[0],
                                  model.coords[quads[i].indices[0]].y +
                                  model.bones[model.coords[quads[i].indices[0]].bone_index].coords[1],
                                  model.coords[quads[i].indices[0]].z +
                                  model.bones[model.coords[quads[i].indices[0]].bone_index].coords[2]
        );
        plSetMeshVertexNormal3f(model.tri_mesh, cur_vert, normal.x, normal.y, normal.z);
        srand(model.coords[quads[i].indices[0]].bone_index);
        r = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        g = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        b = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        plSetMeshVertexColour(model.tri_mesh, cur_vert, plCreateColour4b(r, g, b, 255));
        cur_vert++;

        plSetMeshVertexPosition3f(model.tri_mesh, cur_vert,
                                  model.coords[quads[i].indices[1]].x +
                                  model.bones[model.coords[quads[i].indices[1]].bone_index].coords[0],
                                  model.coords[quads[i].indices[1]].y +
                                  model.bones[model.coords[quads[i].indices[1]].bone_index].coords[1],
                                  model.coords[quads[i].indices[1]].z +
                                  model.bones[model.coords[quads[i].indices[1]].bone_index].coords[2]
        );
        plSetMeshVertexNormal3f(model.tri_mesh, cur_vert, normal.x, normal.y, normal.z);
        srand(model.coords[quads[i].indices[1]].bone_index);
        r = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        g = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        b = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        plSetMeshVertexColour(model.tri_mesh, cur_vert, plCreateColour4b(r, g, b, 255));
        cur_vert++;

        plSetMeshVertexPosition3f(model.tri_mesh, cur_vert,
                                  model.coords[quads[i].indices[2]].x +
                                  model.bones[model.coords[quads[i].indices[2]].bone_index].coords[0],
                                  model.coords[quads[i].indices[2]].y +
                                  model.bones[model.coords[quads[i].indices[2]].bone_index].coords[1],
                                  model.coords[quads[i].indices[2]].z +
                                  model.bones[model.coords[quads[i].indices[2]].bone_index].coords[2]
        );
        plSetMeshVertexNormal3f(model.tri_mesh, cur_vert, normal.x, normal.y, normal.z);
        srand(model.coords[quads[i].indices[2]].bone_index);
        r = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        g = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        b = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        plSetMeshVertexColour(model.tri_mesh, cur_vert, plCreateColour4b(r, g, b, 255));
        cur_vert++;

        va = plCreateVector3D(model.coords[quads[i].normal[2]].x,
                                         model.coords[quads[i].normal[2]].y,
                                         model.coords[quads[i].normal[2]].z);
        vb = plCreateVector3D(model.coords[quads[i].normal[3]].x,
                                         model.coords[quads[i].normal[3]].y,
                                         model.coords[quads[i].normal[3]].z);
        vc = plCreateVector3D(model.coords[quads[i].normal[0]].x,
                                         model.coords[quads[i].normal[0]].y,
                                         model.coords[quads[i].normal[0]].z);
        normal = plGenerateVertexNormal(va, vb, vc);

        plSetMeshVertexPosition3f(model.tri_mesh, cur_vert,
                                  model.coords[quads[i].indices[2]].x +
                                  model.bones[model.coords[quads[i].indices[2]].bone_index].coords[0],
                                  model.coords[quads[i].indices[2]].y +
                                  model.bones[model.coords[quads[i].indices[2]].bone_index].coords[1],
                                  model.coords[quads[i].indices[2]].z +
                                  model.bones[model.coords[quads[i].indices[2]].bone_index].coords[2]
        );
        plSetMeshVertexNormal3f(model.tri_mesh, cur_vert, normal.x, normal.y, normal.z);
        srand(model.coords[quads[i].indices[2]].bone_index);
        r = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        g = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        b = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        plSetMeshVertexColour(model.tri_mesh, cur_vert, plCreateColour4b(r, g, b, 255));
        cur_vert++;

        plSetMeshVertexPosition3f(model.tri_mesh, cur_vert,
                                  model.coords[quads[i].indices[3]].x +
                                  model.bones[model.coords[quads[i].indices[3]].bone_index].coords[0],
                                  model.coords[quads[i].indices[3]].y +
                                  model.bones[model.coords[quads[i].indices[3]].bone_index].coords[1],
                                  model.coords[quads[i].indices[3]].z +
                                  model.bones[model.coords[quads[i].indices[3]].bone_index].coords[2]
        );
        plSetMeshVertexNormal3f(model.tri_mesh, cur_vert, normal.x, normal.y, normal.z);
        srand(model.coords[quads[i].indices[3]].bone_index);
        r = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        g = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        b = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        plSetMeshVertexColour(model.tri_mesh, cur_vert, plCreateColour4b(r, g, b, 255));
        cur_vert++;

        plSetMeshVertexPosition3f(model.tri_mesh, cur_vert,
                                  model.coords[quads[i].indices[0]].x +
                                  model.bones[model.coords[quads[i].indices[0]].bone_index].coords[0],
                                  model.coords[quads[i].indices[0]].y +
                                  model.bones[model.coords[quads[i].indices[0]].bone_index].coords[1],
                                  model.coords[quads[i].indices[0]].z +
                                  model.bones[model.coords[quads[i].indices[0]].bone_index].coords[2]
        );
        plSetMeshVertexNormal3f(model.tri_mesh, cur_vert, normal.x, normal.y, normal.z);
        srand(model.coords[quads[i].indices[0]].bone_index);
        r = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        g = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        b = (uint8_t)((rand() % 255) * ((rand() % 254) + 1));
        plSetMeshVertexColour(model.tri_mesh, cur_vert, plCreateColour4b(r, g, b, 255));
    }
#endif
    plUploadMesh(model.tri_mesh);

    CLEANUP:
    fclose(file);
}

enum {
    VIEW_MODE_LIT,
    VIEW_MODE_WIREFRAME,
    VIEW_MODE_POINTS,
    VIEW_MODE_WEIGHTS,
    VIEW_MODE_SKELETON
};
int view_mode = VIEW_MODE_LIT;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {

    switch(key) {
        default: break;

        case GLFW_KEY_1: {
            if(action == GLFW_PRESS) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                view_mode = VIEW_MODE_LIT;
            }
            break;
        }
        case GLFW_KEY_2: {
            if((action == GLFW_PRESS) && (mode != VIEW_MODE_WIREFRAME)) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDisable(GL_LIGHTING);
                view_mode = VIEW_MODE_WIREFRAME;
            }
            break;
        }
        case GLFW_KEY_3: {
            if((action == GLFW_PRESS) && (mode != VIEW_MODE_POINTS)) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDisable(GL_LIGHTING);
                view_mode = VIEW_MODE_POINTS;
            }
            break;
        }
        case GLFW_KEY_4: {
            if((action == GLFW_PRESS) && (mode != VIEW_MODE_WEIGHTS)) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                view_mode = VIEW_MODE_WEIGHTS;
            }
            break;
        }
        case GLFW_KEY_5: {
            if((action == GLFW_PRESS) && (mode != VIEW_MODE_SKELETON)) {
                view_mode = VIEW_MODE_SKELETON;
            }
            break;
        }

        case GLFW_KEY_TAB: {
            if(action == GLFW_PRESS) {
                g_state.is_psx_mode = !(g_state.is_psx_mode);
                if(!g_state.is_psx_mode) {
                    glfwGetFramebufferSize(window,
                                           (int*)&g_state.main_camera->viewport.w,
                                           (int*)&g_state.main_camera->viewport.h);
                } else {
                    g_state.main_camera->viewport.w = PSX_WIDTH;
                    g_state.main_camera->viewport.h = PSX_HEIGHT;
                }
            }
            break;
        }

        case GLFW_KEY_ESCAPE: {
            if(action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }
            break;
        }
    }
}

/* Hogs of War directory structure...
 *     Audio
 *     Chars
 *     FEBmps
 *     FESounds
 *     FEText
 *     Maps
 *     Skys
 *     Speech
 */

GlobalVars g_state;

void DrawOverlays(void) {
#if 1
    DrawText(fonts[FONT_BIG], 10, 10, 4, "abcd ABCD");
#else
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ONE);

    glBindTexture(GL_TEXTURE_2D, fonts[FONT_BIG]->texture);

    DrawCharacter(fonts[FONT_BIG], 100, 100, 2.f, '!');

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
#endif
}

void DrawScene(void) {

}

void copy_map_file(const char *path) {
    char file_path[PL_SYSTEM_MAX_PATH];
    snprintf(file_path, sizeof(file_path), "./data/maps/%s", plGetFileName(path));
    plLowerCasePath(file_path);
    plCopyFile(path, file_path);
}

int main(int argc, char **argv) {
    memset(&g_state, 0, sizeof(GlobalVars));

    plInitialize(argc, argv, PL_SUBSYSTEM_LOG);
    plClearLog(LOG);

    PRINT("\n " TITLE " : Version %d.%d (" __DATE__ ")\n", VERSION_MAJOR, VERSION_MINOR     );
    PRINT(" Developed by...\n"                                                              );
    PRINT("   Mark \"hogsy\" Sowden (http://talonbrave.info/)\n"                            );
    PRINT("   Daniel \"solemnwarning\" Collins (http://solemnwarning.net/)\n"               );
    PRINT("\n-------------------------------------------------------------------------\n\n" );

    // Initialize GLFW...

    if (!glfwInit()) {
        plMessageBox(TITLE, "Failed to initialize GLFW!\n");
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    g_state.width = WIDTH; g_state.height = HEIGHT;
    GLFWwindow *window = glfwCreateWindow(g_state.width, g_state.height, "Piggy Viewer", NULL, NULL);
    if (!window) {
        glfwTerminate();

        plMessageBox(TITLE, "Failed to create window!\n");
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

#if 0
    glewExperimental=TRUE;
    glewInit();
#endif

    // Initialize DevIL...

    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);

    ilEnable(IL_CONV_PAL);

    // And now for ours...

    // todo, move all of this out into separate setup executable

    // Check if it's the PSX content or PC content.
    if(plFileExists("./system.cnf")) {
        PRINT("Found system.cnf, assuming PSX version...\n");
        g_state.is_psx = true;
    }

    if(!plPathExists("./" PORK_BASE_DIR)) {
        DPRINT("Unable to find data directory, extracting game contents...\n");
        if (
                !plCreateDirectory("./" PORK_BASE_DIR)          ||
                !plCreateDirectory("./" PORK_BASE_DIR "/chars") ||
                !plCreateDirectory("./" PORK_BASE_DIR "/maps")) {
            PRINT_ERROR("Failed to create base directory for data!\n");
        }

        InitializeMADPackages();

        // Copy all of the map files over into a new directory.
        plScanDirectory("./Maps/", ".pog", copy_map_file, false);
        plScanDirectory("./Maps/", ".pmg", copy_map_file, false);
        plScanDirectory("./Maps/", ".ptg", copy_map_file, false);
        plScanDirectory("./Maps/", ".gen", copy_map_file, false);

        plCopyFile("./Chars/pig.HIR", "./" PORK_BASE_DIR "/pig.hir");
    } else {
        DPRINT("Found data directory, continuing with normal execution...\n");
    }

    //LoadPOG("./data/maps/tester.pog");
    //plScanDirectory("./data/maps/", ".pog", LoadPOG, false);

    init_model_cache();

    InitializeFonts();
    init_objects();

    ////////////////////////////////////////////////////

    plInitialize(argc, argv, PL_SUBSYSTEM_GRAPHICS);

    plSetDefaultGraphicsState();
    plSetClearColour(plCreateColour4b(0, 0, 128, 255));

    plSetupConsole(1);
    plShowConsole(true);
    plSetConsoleColour(1, plCreateColour4b(128, 0, 0, 128));

#if 1
    glGenFramebuffers(1, &g_state.gl.psx_fbo);
    glGenRenderbuffers(1, &g_state.gl.colour_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, g_state.gl.colour_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB4, PSX_WIDTH, PSX_HEIGHT);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_state.gl.psx_fbo);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, g_state.gl.colour_rbo);

    glGenRenderbuffers(1, &g_state.gl.depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, g_state.gl.depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, PSX_WIDTH, PSX_HEIGHT);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_state.gl.depth_rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
#if 0 // bring this back at some stage...
        PRINT(glErrorStringREGAL(glGetError()));
#endif
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
#endif

    g_state.main_camera = plCreateCamera();
    if (!g_state.main_camera) {
        PRINT_ERROR("Failed to create camera!\n");
    }
    g_state.main_camera->mode = PLCAMERA_MODE_PERSPECTIVE;
    g_state.main_camera->fov = 90.f;
    glfwGetFramebufferSize(window,
                           (int*)&g_state.main_camera->viewport.w,
                           (int*)&g_state.main_camera->viewport.h);

    PLCamera *camera1 = plCreateCamera();
    if(!camera1) {
        PRINT_ERROR("Failed to create secondary camera!\n");
    }
    camera1->mode       = PLCAMERA_MODE_ORTHOGRAPHIC;
    camera1->viewport.w = g_state.main_camera->viewport.w;
    camera1->viewport.h = g_state.main_camera->viewport.h;

    const char *arg;
    if ((arg = plGetCommandLineArgument("-model")) && (arg[0] != '\0')) {
        memset(&model, 0, sizeof(PIGModel));

        char vtx_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        snprintf(vtx_path, sizeof(vtx_path), "./%s.vtx", arg);
        char fac_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        snprintf(fac_path, sizeof(fac_path), "./%s.fac", arg);
        char no2_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        snprintf(no2_path, sizeof(no2_path), "./%s.no2", arg);

        load_hir_file("./Chars/pig.HIR");
        load_vtx_file(vtx_path);
        load_fac_file(fac_path);

        g_state.main_camera->position = plCreateVector3D(0, 12, -500);

        glEnable(GL_CULL_FACE);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

#if 0
        // ol' gl lighting, just for testing
        GLfloat light_ambient[] = {0.6f, 0.6f, 0.6f, 1.f};
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        GLfloat light0_position[] = {12.f, 0, 800.f};
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
        glEnable(GL_LIGHT1);
        GLfloat light_colour_red[] = {1.5f, 0.5f, 0.5f, 1.f};
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light_colour_red);
        GLfloat light_position[] = {0, 12.f, -800.f};
        glLightfv(GL_LIGHT1, GL_POSITION, light_position);
#else
        PLLight light[4];
        memset(&light, 0, sizeof(PLLight) * 4);
        light[0].position   = plCreateVector3D(0, 12.f, -800.f);
        light[0].colour     = plCreateColour4f(1.5f, .5f, .5f, 128.f);
        light[0].type       = PLLIGHT_TYPE_OMNI;
#endif

        glPointSize(5.f);
        glLineWidth(2.f);

        while (!glfwWindowShouldClose(window)) {

            glfwPollEvents();

            // Logic start

            process_objects();

            // Rendering start

            DrawScene();

            if(g_state.is_psx_mode) {
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_state.gl.psx_fbo);
            } else {
                glEnable(GL_LINE_SMOOTH);
            }

            plClearBuffers(PL_BUFFER_COLOUR | PL_BUFFER_DEPTH | PL_BUFFER_STENCIL);

            // input handlers start..
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            // Camera rotation
            static double oldmpos[2] = {0, 0};
            int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            if (state == GLFW_PRESS) {
                double nxpos = xpos - oldmpos[0];
                double nypos = ypos - oldmpos[1];
                model.angles.x += (nxpos / 100.f);
                model.angles.y += (nypos / 100.f);
            } else {
                oldmpos[0] = xpos;
                oldmpos[1] = ypos;
            }

            // Zoom in and out thing...
            static double oldrmpos[2] = {0, 0};
            state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
            if (state == GLFW_PRESS) {
                double nypos = ypos - oldrmpos[1];
                g_state.main_camera->position.z += (nypos / 20.f);
            } else {
                oldrmpos[0] = xpos;
                oldrmpos[1] = ypos;
            }
            // input handlers end...

            plSetupCamera(g_state.main_camera);

            draw_objects();

#if 1
            glLoadIdentity();

            glPushMatrix();
            glRotatef(model.angles.y, 1, 0, 0);
            glRotatef(model.angles.x, 0, 1, 0);
            glRotatef(model.angles.z + 180.f, 0, 0, 1);

            switch (view_mode) {
                default:
                    break;

                case VIEW_MODE_LIT: {
                    glEnable(GL_LIGHTING);
                    glShadeModel(GL_FLAT);

                    plDrawMesh(model.tri_mesh);

                    glShadeModel(GL_SMOOTH);
                    glDisable(GL_LIGHTING);
                    break;
                }

                case VIEW_MODE_WEIGHTS:
                case VIEW_MODE_WIREFRAME: {
                    plDrawMesh(model.tri_mesh);
                    break;
                }

                case VIEW_MODE_SKELETON: {
                    plDrawMesh(model.tri_mesh);
                    glDisable(GL_DEPTH_TEST);
                    plDrawMesh(model.skeleton_mesh);
                    glEnable(GL_DEPTH_TEST);
                }
            }

            glPopMatrix();
#endif

            plSetupCamera(camera1);

            DrawOverlays();

            plDrawConsole();

            if(g_state.is_psx_mode) {
                static uint8_t *psx_buffer = NULL;
                if(!psx_buffer) {
                    psx_buffer = (uint8_t*)malloc(PSX_WIDTH * PSX_HEIGHT * 4);
                }

                glReadBuffer(GL_COLOR_ATTACHMENT0);
                glReadPixels(0, 0, PSX_WIDTH, PSX_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, &psx_buffer[0]);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

                plScissor(0, 0, WIDTH, HEIGHT);

                glBindFramebuffer(GL_READ_FRAMEBUFFER, g_state.gl.psx_fbo);
                glBlitFramebuffer(0, 0, PSX_WIDTH, PSX_HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            } else {
                glDisable(GL_LINE_SMOOTH);
            }

            glfwSwapBuffers(window);
        }

        plDeleteMesh(model.tri_mesh);
        plDeleteMesh(model.skeleton_mesh);

        glfwTerminate();
    }

    plDeleteCamera(g_state.main_camera);

    plShutdown();

    return 0;
}