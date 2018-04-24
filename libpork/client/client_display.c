/* OpenHoW
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
#include <PL/platform_graphics_camera.h>
#include <PL/platform_filesystem.h>

#include "pork_engine.h"
#include "pork_input.h"
#include "pork_console.h"
#include "pork_map.h"

#include "client_font.h"
#include "client_actor.h"
#include "client_frontend.h"
#include "client_shader.h"

/************************************************************/
/* PORK TEXTURE CACHE                                       */

PLConsoleVariable *cv_display_texture_cache = NULL;

enum {
    INDEX_BRITISH,
    INDEX_AMERICAN,
    INDEX_FRENCH,
    INDEX_GERMAN,
    INDEX_RUSSIAN,
    INDEX_JAPANESE,
    INDEX_TEAMLARD,

    INDEX_GOBS,

    INDEX_WEAPONS,
    INDEX_MAP,

    MAX_TEXTURE_INDEX
};

#define MAX_TEXTURES_PER_INDEX  1024

typedef struct TextureIndex {
    struct {
        int x, y;
        unsigned int w, h;
    } offsets[MAX_TEXTURES_PER_INDEX];
    unsigned int num_textures;

    PLTexture *texture;
} TextureIndex;
TextureIndex texture_cache[MAX_TEXTURE_INDEX];

size_t GetTextureCacheSize(void) {
    size_t size = 0;
    for(unsigned int i = 0; i < MAX_TEXTURE_INDEX; ++i) {
        TextureIndex *index = &texture_cache[i];
        if(index->texture == NULL) {
            continue;
        }
        size += plGetImageSize(index->texture->format, index->texture->w, index->texture->h);
    }
    return size;
}

void GetCachedTextureCoords(unsigned int id, unsigned int tex_id, int *x, int *y, unsigned int *w, unsigned int *h) {
    pork_assert(id < MAX_TEXTURE_INDEX && tex_id < MAX_TEXTURES_PER_INDEX);
    TextureIndex *index = &texture_cache[id];
    *x = index->offsets[tex_id].x;
    *y = index->offsets[tex_id].y;
    *w = index->offsets[tex_id].w;
    *h = index->offsets[tex_id].h;
}

void PrintTextureCacheSizeCommand(unsigned int argc, char *argv[]) {
    size_t cache_size = GetTextureCacheSize();
    const char *str = "total texture cache: ";
    if(argc < 2) {
        LogInfo("%s %u bytes", str, cache_size);
        return;
    }

    if(pl_strncasecmp(argv[1], "KB", 2) == 0) {
        LogInfo("%s %.2fKB (%u bytes)\n", str, plBytesToKilobytes(cache_size), cache_size);
    } else if(pl_strncasecmp(argv[1], "MB", 2) == 0) {
        LogInfo("%s %.2fMB (%u bytes)\n", str, plBytesToMegabytes(cache_size), cache_size);
    } else if(pl_strncasecmp(argv[1], "GB", 2) == 0) {
        LogInfo("%s %.2fGB (%u bytes)\n", str, plBytesToGigabytes(cache_size), cache_size);
    } else {
        LogInfo("unknown parameter \"%s\", ignoring!\n", argv[1]);
    }
}

/* for debugging purposes, displays the textures
 * cached in memory on the screen */
void DrawTextureCache(unsigned int id) {
    pork_assert(id < MAX_TEXTURE_INDEX);
    TextureIndex *index = &texture_cache[id];
    if(index->num_textures > 0) {
        unsigned int w = index->texture->w;
        unsigned int h = index->texture->h;
        if (w > g_state.display_width) {
            w = g_state.display_width;
        }
        if (h > g_state.display_height) {
            h = g_state.display_height;
        }

        plDrawTexturedRectangle(0, 0, w, h, index->texture);
#if 1
        for(unsigned int i = 0; i < index->num_textures; ++i) {
            plDrawRectangle(
                    index->offsets[i].x,
                    index->offsets[i].y,
                    index->offsets[i].w,
                    index->offsets[i].h,
                    PL_COLOUR_RED
            );
        }
#endif
        char msg[256];
        snprintf(msg, sizeof(msg), "%u TEXTURES\n%dX%d\n%.2fMB (VRAM)",
                 index->num_textures,
                 index->texture->w,
                 index->texture->h,
                 plBytesToMegabytes(index->texture->size));
        DrawBitmapString(g_fonts[FONT_SMALL], 10, 10, 0, 1.f, PL_COLOUR_WHITE, msg);
    } else {
        DrawBitmapString(g_fonts[FONT_SMALL], 10, 10, 0, 1.f, PL_COLOUR_WHITE, "NO DATA CACHED!");
    }
}

/* unloads texture set from memory */
void ClearTextureIndex(unsigned int id) {
    pork_assert(id < MAX_TEXTURE_INDEX);
    TextureIndex *index = &texture_cache[id];
    plDeleteTexture(index->texture, true);
    memset(index, 0, sizeof(TextureIndex));
}

int CompareImageHeight(const void *a, const void *b) {
    const PLImage *img_a = (const PLImage*)a;
    const PLImage *img_b = (const PLImage*)b;
    if(img_a->height < img_b->height) {
        return 1;
    } else if(img_a->height > img_b->height) {
        return -1;
    }

    return 0;
}

/* loads texture set into memory */
void CacheTextureIndex(const char *path, const char *index_name, unsigned int id) {
    pork_assert(id < MAX_TEXTURE_INDEX);
    TextureIndex *index = &texture_cache[id];
    if(index->num_textures > 0) {
        LogWarn("textures already cached for index %s\n", index_name);
        return;
    }

    char texture_index_path[PL_SYSTEM_MAX_PATH];
    snprintf(texture_index_path, sizeof(texture_index_path), "%s%s%s", GetBasePath(), path, index_name);
    if(!plFileExists(texture_index_path)) {
        Error("failed to find index at \"%s\", aborting!\n", texture_index_path);
    }

    FILE *file = fopen(texture_index_path, "r");
    if(file == NULL) {
        Error("failed to open texture index at \"%s\", aborting!\n", texture_index_path);
    }

    LogInfo("parsing \"%s\"\n", texture_index_path);

    /* todo, move all textures into a texture sheet on upload */

    PLImage images[MAX_TEXTURES_PER_INDEX];

    unsigned int w = 256;
    unsigned int h = 8;
    unsigned int max_h = 0;
    int cur_y = 0;
    int cur_x = 0;

    char line[32];
    while(!feof(file)) {
        if(fgets(line, sizeof(line), file) != NULL) {
            line[strcspn(line, "\r\n")] = '\0';
            //LogDebug("  %s\n", line);

            char texture_path[PL_SYSTEM_MAX_PATH];
            snprintf(texture_path, sizeof(texture_path), "%s%s%s.tim", GetBasePath(), path, line);
            if(!plFileExists(texture_path)) {
                /* check for PNG variant */
                snprintf(texture_path, sizeof(texture_path), "%s%s%s.png", GetBasePath(), path, line);
            }

            if(index->num_textures >= MAX_TEXTURES_PER_INDEX) {
                Error("hit max index (%u) for texture sheet, aborting!\n", MAX_TEXTURES_PER_INDEX);
            }

            PLImage *img = &images[index->num_textures];
            if(!plLoadImage(texture_path, img)) {
                Error("%s, aborting!\n", plGetError());
            }

            plConvertPixelFormat(img, PL_IMAGEFORMAT_RGBA8);

            index->num_textures++;
        }
    }

    qsort(images, index->num_textures, sizeof(PLImage), &CompareImageHeight);

    /* copy the information into our index list, so we can easily
     * grab it later */

    for(unsigned int i = 0; i < index->num_textures; ++i) {
        PLImage *img = &images[i];

        if(img->height > max_h) {
            max_h = img->height;
        }

        if(cur_x == 0 && img->width > w) {
            w = img->width;
        } else if(cur_x + img->width > w) {
            cur_y += max_h;
            cur_x = 0;
            max_h = 0;
        }

        /* todo, perhaps, if the texture sheet is too large we should create another sheet?
         * rather than producing one mega texture */
        while(cur_y + img->height > h) {
            h *= 2;
        }

        index->offsets[i].x = cur_x;
        index->offsets[i].y = cur_y;
        index->offsets[i].w = img->width;
        index->offsets[i].h = img->height;
        cur_x += img->width;
    }

    PLImage cache;
    memset(&cache, 0, sizeof(PLImage));
    cache.width           = w;
    cache.height          = h;
    cache.format          = PL_IMAGEFORMAT_RGBA8;
    cache.colour_format   = PL_COLOURFORMAT_RGBA;
    cache.levels          = 1;
    cache.size            = plGetImageSize(cache.format, cache.width, cache.height);

    cache.data = pork_alloc(cache.levels, sizeof(uint8_t*), true);
    cache.data[0] = pork_alloc(cache.size, sizeof(uint8_t), true);
    for(unsigned int i = 0; i < index->num_textures; ++i) {
        uint8_t *pos = cache.data[0] + ((index->offsets[i].y * cache.width) + index->offsets[i].x) * 4;
        uint8_t *src = images[i].data[0];
        for(unsigned int y = 0; y < index->offsets[i].h; ++y) {
            memcpy(pos, src, (index->offsets[i].w * 4));
            src += index->offsets[i].w * 4;
            pos += cache.width * 4;
        }

        plFreeImage(&images[i]);
    }

#if 0 /* experimental palette changer thing... */
    PLColour main_channel = PLColourRGB((uint8_t) (rand() % 255), (uint8_t) (rand() % 255), (uint8_t) (rand() % 255));
    plReplaceImageColour(&cache, PLColourRGB(90, 82, 8), main_channel);

    PLColour dark_channel = main_channel;
    if(dark_channel.r > 16) dark_channel.r -= 16;
    if(dark_channel.g > 25) dark_channel.g -= 25;
    if(dark_channel.b > 8)  dark_channel.b -= 8;
    plReplaceImageColour(&cache, PLColourRGB(74, 57, 0), dark_channel);

    PLColour light_channel = main_channel;
    if(light_channel.r < 206) light_channel.r += 49;
    if(light_channel.g < 197) light_channel.g += 58;
    if(light_channel.b < 214) light_channel.b += 41;
    plReplaceImageColour(&cache, PLColourRGB(139, 115, 49), light_channel);

    PLColour mid_channel = main_channel;
    /* this one still needs doing... */
    plReplaceImageColour(&cache, PLColourRGB(115, 98, 24), mid_channel);
#endif

    if((index->texture = plCreateTexture()) == NULL) {
        Error("%s, aborting!\n", plGetError());
    }

    index->texture->filter = PL_TEXTURE_FILTER_NEAREST;
    if(!plUploadTextureImage(index->texture, &cache)) {
        Error("%s, aborting!\n", plGetError());
    }

    pork_fclose(file);
}

PLTexture *LoadTexture(const char *path, PLTextureFilter filter) {
    char tpath[PL_SYSTEM_MAX_PATH];
    snprintf(tpath, sizeof(tpath), "%s%s", GetBasePath(), path);

    PLTexture *texture = NULL;

    const char *ext = plGetFileExtension(path);
    if(ext != NULL && ext[0] != '\0' && ext[0] != ' ') {
        /* pixel format of TIM will be changed before
         * uploading */
        if(pl_strncasecmp(ext, "tim", 3) == 0) {
            PLImage img;
            if(!plLoadImage(tpath, &img)) {
                Error("%s, aborting!\n", plGetError());
            }
            plConvertPixelFormat(&img, PL_IMAGEFORMAT_RGBA8);
            if((texture = plCreateTexture()) != NULL) {
                texture->filter = filter;
                if(!plUploadTextureImage(texture, &img)) {
                    Error("%s, aborting!\n", plGetError());
                }
                return texture;
            }
            Error("%s, aborting!\n", plGetError());
        }
    }

    texture = plLoadTextureImage(tpath, PL_TEXTURE_FILTER_LINEAR);
    if(texture == NULL) {
        Error("%s, aborting!\n", plGetError());
    }

    return texture;
}

/************************************************************/

void InitDisplay(void) {
    if(g_launcher.DisplayWindow) {
        g_launcher.DisplayWindow(g_state.display_fullscreen, g_state.display_width, g_state.display_height);
    }

    plInitializeSubSystems(PL_SUBSYSTEM_GRAPHICS);
    plSetGraphicsMode(PL_GFX_MODE_OPENGL);

    InitShaders();

    //////////////////////////////////////////////////////////

    plSetClearColour(PLColour(0, 0, 0, 255));

    g_state.camera = plCreateCamera();
    if(g_state.camera == NULL) {
        Error("failed to create camera, aborting!\n%s\n", plGetError());
    }
    g_state.camera->mode        = PL_CAMERA_MODE_PERSPECTIVE;
    g_state.camera->bounds      = (PLAABB){{-20, -20},{20, 20}};
    g_state.camera->fov         = 90;
    g_state.camera->viewport.w  = g_state.display_width;
    g_state.camera->viewport.h  = g_state.display_height;

    g_state.ui_camera = plCreateCamera();
    if(g_state.ui_camera == NULL) {
        Error("failed to create ui camera, aborting!\n%s\n", plGetError());
    }
    g_state.ui_camera->mode         = PL_CAMERA_MODE_ORTHOGRAPHIC;
    g_state.ui_camera->fov          = 90;
    g_state.ui_camera->near         = 0;
    g_state.ui_camera->far          = 1000;
    g_state.ui_camera->viewport.w   = g_state.display_width;
    g_state.ui_camera->viewport.h   = g_state.display_height;
    g_state.ui_camera->viewport.r_w = 640;
    g_state.ui_camera->viewport.r_h = 480;

    //plSetCullMode(PL_CULL_POSTIVE);

    /* initialize the texture cache */

    LogInfo("caching texture groups...\n");

    CacheTextureIndex("/chars/american/", "american.index", INDEX_AMERICAN);
    CacheTextureIndex("/chars/british/", "british.index", INDEX_BRITISH);
    CacheTextureIndex("/chars/french/", "french.index", INDEX_FRENCH);
    CacheTextureIndex("/chars/german/", "german.index", INDEX_GERMAN);
    CacheTextureIndex("/chars/japanese/", "japanese.index", INDEX_JAPANESE);
    CacheTextureIndex("/chars/russian/", "russian.index", INDEX_RUSSIAN);
    CacheTextureIndex("/chars/teamlard/", "teamlard.index", INDEX_TEAMLARD);

    CacheTextureIndex("/chars/weapons/", "weapons.index", INDEX_WEAPONS);

    PrintTextureCacheSizeCommand(2, (char*[]){"", "MB"});

    plRegisterConsoleCommand("printtcache", PrintTextureCacheSizeCommand, "displays current texture memory usage");
    cv_display_texture_cache = plRegisterConsoleVariable("displaytcache", "-1", pl_int_var, NULL, "");
}

void ShutdownDisplay(void) {
    ShutdownShaders();
}

/* shared function */
void UpdatePorkViewport(bool fullscreen, unsigned int width, unsigned int height) {
    if(g_state.camera == NULL || g_state.ui_camera == NULL) {
        // display probably hasn't been initialised
        return;
    }

    g_state.ui_camera->viewport.w = g_state.camera->viewport.w = width;
    g_state.ui_camera->viewport.h = g_state.camera->viewport.h = height;

    ResetInputStates();
}

/************************************************************/

void DEBUGDrawSkeleton();

void DrawDebugOverlay(void) {
    if(cv_debug_mode->i_value <= 0) {
        return;
    }

    if(cv_display_texture_cache->i_value > -1) {
        DrawTextureCache((unsigned int) cv_display_texture_cache->i_value);
    }

    if (cv_debug_fps->b_value) {
        static unsigned int fps = 0;
        static unsigned int ms = 0;
        static unsigned int update_delay = 60;
        if (update_delay < g_state.draw_ticks && g_state.last_draw_ms > 0) {
            ms = g_state.last_draw_ms;
            fps = 1000 / ms;
            update_delay = g_state.draw_ticks + 60;
        }

        char ms_count[32];
        sprintf(ms_count, "FPS: %d (%d)", fps, ms);
        DrawBitmapString(g_fonts[FONT_GAME_CHARS], 20, GetViewportHeight() - 32, 0, 1.f, PL_COLOUR_WHITE, ms_count);
    }

    if (cv_debug_input->i_value > 0) {
        switch (cv_debug_input->i_value) {
            default: {
                DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "KEYBOARD STATE");
                unsigned int x = 20, y = 50;
                for (unsigned int i = 0; i < PORK_MAX_KEYS; ++i) {
                    bool status = GetKeyState(i);
                    char key_state[64];
                    snprintf(key_state, sizeof(key_state), "%d (%s)", i, status ? "TRUE" : "FALSE");
                    DrawBitmapString(g_fonts[FONT_SMALL], x, y, 0, 1.f, PL_COLOUR_WHITE, key_state);
                    if (y + 15 > GetViewportHeight() - 50) {
                        x += 90;
                        y = 50;
                    } else {
                        y += 15;
                    }
                }
            }
                break;

            case 2: {
                DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "CONTROLLER STATE");

                char button_state[64];
                snprintf(button_state, sizeof(button_state), CHAR_PSX_CROSS " (%s)",
                         GetButtonState(0, PORK_BUTTON_CROSS) ? "TRUE" : "FALSE");
                unsigned int y = 50;
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_TRIANGLE " (%s)",
                         GetButtonState(0, PORK_BUTTON_TRIANGLE) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_CIRCLE " (%s)",
                         GetButtonState(0, PORK_BUTTON_CIRCLE) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_SQUARE " (%s)",
                         GetButtonState(0, PORK_BUTTON_SQUARE) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_L1 " (%s)",
                         GetButtonState(0, PORK_BUTTON_L1) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_L2 " (%s)",
                         GetButtonState(0, PORK_BUTTON_L2) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_R1 " (%s)",
                         GetButtonState(0, PORK_BUTTON_R1) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_R2 " (%s)",
                         GetButtonState(0, PORK_BUTTON_R2) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "START (%s)",
                         GetButtonState(0, PORK_BUTTON_START) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "SELECT (%s)",
                         GetButtonState(0, PORK_BUTTON_SELECT) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "UP (%s)",
                         GetButtonState(0, PORK_BUTTON_UP) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "DOWN (%s)",
                         GetButtonState(0, PORK_BUTTON_DOWN) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "LEFT (%s)",
                         GetButtonState(0, PORK_BUTTON_LEFT) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "RIGHT (%s)",
                         GetButtonState(0, PORK_BUTTON_RIGHT) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);
            } break;
        }
        return;
    }

#if 1
    DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "CAMERA");
    unsigned int y = 50;
    char cam_pos[32];
    snprintf(cam_pos, sizeof(cam_pos), "POSITION : %s", plPrintVector3(g_state.camera->position));
    DrawBitmapString(g_fonts[FONT_SMALL], 20, y, 0, 1.f, PL_COLOUR_WHITE, cam_pos);
    snprintf(cam_pos, sizeof(cam_pos), "ANGLES   : %s", plPrintVector3(g_state.camera->angles));
    DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, cam_pos);
#endif
}

/* shared function */
void DrawPork(double delta) {
    g_state.draw_ticks = g_launcher.GetTicks();

    unsigned int clear_flags = PL_BUFFER_DEPTH;
    if(GetFrontendState() == FE_MODE_GAME || cv_debug_mode->i_value > 0) {
        clear_flags |= PL_BUFFER_COLOUR;
    }
    plClearBuffers(clear_flags);

    if(
            GetFrontendState() != FE_MODE_INIT &&
            GetFrontendState() != FE_MODE_LOADING) {
        plSetupCamera(g_state.camera);

        if(GetFrontendState() == FE_MODE_GAME) {
            DrawMap();
        }

        DrawActors(delta);
    }

    plSetupCamera(g_state.ui_camera);

    DrawFrontend();

    // todo, need a better name for this function
    plDrawPerspectivePOST(g_state.ui_camera);

    if(
            GetFrontendState() != FE_MODE_INIT &&
            GetFrontendState() != FE_MODE_LOADING) {
        DrawDebugOverlay();
        DrawConsole();
    }

    g_launcher.SwapWindow();

    g_state.last_draw_ms = g_launcher.GetTicks() - g_state.draw_ticks;
}

/**************************************************************************************/

