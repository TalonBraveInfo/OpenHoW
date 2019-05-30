/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include "../engine.h"
#include "../input.h"
#include "../imgui_layer.h"
#include "../frontend.h"
#include "../particle.h"
#include "../loaders/loaders.h"

#include "font.h"
#include "shader.h"
#include "display.h"

/************************************************************/
/* Texture Cache */

#define MAX_TEXTURES_PER_INDEX  1024
#define MAX_VIDEO_PRESETS 32

static int num_vid_presets;
static VideoPreset vid_presets[MAX_VIDEO_PRESETS];
static PLFrameBuffer* game_target;
static PLFrameBuffer* frontend_target;

static PLTexture* default_texture = NULL;

PLTexture* Display_GetDefaultTexture(void) {
    return default_texture;
}

typedef struct TextureIndex {
    struct {
        int x, y;
        unsigned int w, h;
    } offsets[MAX_TEXTURES_PER_INDEX];
    unsigned int num_textures;

    PLTexture* texture;
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

void Display_GetCachedTextureCoords(unsigned int id, unsigned int tex_id, float *x, float *y, float *w, float *h) {
    u_assert(id < MAX_TEXTURE_INDEX && tex_id < MAX_TEXTURES_PER_INDEX);
    TextureIndex* index = &texture_cache[id];
    if(index->texture == default_texture) {
        *x = 0; *y = 0; *w = 1.0f; *h = 1.0f;
        return;
    }
    *x = (float)index->offsets[tex_id].x / index->texture->w;
    *y = (float)index->offsets[tex_id].y / index->texture->h;
    *w = (float)index->offsets[tex_id].w / index->texture->w;
    *h = (float)index->offsets[tex_id].h / index->texture->h;
}

PLTexture* Display_GetCachedTexture(unsigned int id) {
    u_assert(id < MAX_TEXTURE_INDEX);
    if(texture_cache[id].texture == NULL) {
        return default_texture;
    }

    return texture_cache[id].texture;
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
    u_assert(id < MAX_TEXTURE_INDEX);
    TextureIndex *index = &texture_cache[id];
    if(index->num_textures > 0) {
        int w = index->texture->w;
        int h = index->texture->h;
        if (w > cv_display_width->i_value) {
            w = (unsigned int) cv_display_width->i_value;
        }
        if (h > cv_display_height->i_value) {
            h = (unsigned int) cv_display_height->i_value;
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
        Font_DrawBitmapString(g_fonts[FONT_SMALL], 10, 10, 0, 1.f, PL_COLOUR_WHITE, msg);
    } else {
        Font_DrawBitmapString(g_fonts[FONT_SMALL], 10, 10, 0, 1.f, PL_COLOUR_WHITE, "NO DATA CACHED!");
    }
}

/* unloads texture set from memory */
void Display_ClearTextureIndex(unsigned int id) {
    u_assert(id < MAX_TEXTURE_INDEX);
    TextureIndex* index = &texture_cache[id];
    if(index->texture != default_texture) {
        plDestroyTexture(index->texture, true);
    }
    memset(index, 0, sizeof(TextureIndex));
    index->texture = default_texture;
}

static int CompareImageHeight(const void *a, const void *b) {
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
void Display_CacheTextureIndex(const char* path, const char* index_name, unsigned int id) {
    u_assert(id < MAX_TEXTURE_INDEX);
    TextureIndex* index = &texture_cache[id];
    if(index->num_textures > 0) {
        Display_ClearTextureIndex(id);
    }

    char tmp[PL_SYSTEM_MAX_PATH];
    snprintf(tmp, sizeof(tmp), "%s%s", path, index_name);

    char texture_index_path[PL_SYSTEM_MAX_PATH];
    snprintf(texture_index_path, sizeof(texture_index_path), "%s", u_find(tmp));

    if(!plFileExists(texture_index_path)) {
        LogWarn("Failed to find index at \"%s\" (%s)!\n", texture_index_path, plGetError());
        return;
    }

    FILE *file = fopen(texture_index_path, "r");
    if(file == NULL) {
        LogWarn("Failed to open texture index at \"%s\"!\n", texture_index_path);
        return;
    }

    LogInfo("Parsing \"%s\"\n", texture_index_path);

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
            snprintf(tmp, sizeof(tmp), "%s%s", path, line);
            snprintf(texture_path, sizeof(texture_index_path), "%s", u_find2(tmp, supported_image_formats));

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

    cache.data = (uint8_t**)u_alloc(cache.levels, sizeof(uint8_t *), true);
    cache.data[0] = (uint8_t*)u_alloc(cache.size, sizeof(uint8_t), true);
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

    u_fclose(file);
}

//const char *supported_model_formats[]={"vtx", NULL};
const char *supported_image_formats[]={"png", "tga", "bmp", "tim", NULL};
//const char *supported_audio_formats[]={"wav", NULL};
//const char *supported_video_formats[]={"bik", NULL};

PLTexture* Display_LoadTexture(const char *path, PLTextureFilter filter) {
    char n_path[PL_SYSTEM_MAX_PATH];
    const char* ext = plGetFileExtension(path);
    if(plIsEmptyString(ext)) {
        strncpy(n_path, u_find2(path, supported_image_formats), sizeof(n_path));
        if(plIsEmptyString(n_path)) {
            LogWarn("Failed to find texture, \"%s\"!\n", path);
            return default_texture;
        }

        PLTexture* texture = plLoadTextureImage(n_path, filter);
        if(texture == NULL) {
            LogWarn("%s, aborting!\n", plGetError());
            return default_texture;
        }

        return texture;
    }

    PLImage img;
    strncpy(n_path, u_find(path), sizeof(n_path));
    if(plLoadImage(n_path, &img)) {
        /* pixel format of TIM will be changed before uploading */
        if(pl_strncasecmp(ext, "tim", 3) == 0) {
            plConvertPixelFormat(&img, PL_IMAGEFORMAT_RGBA8);
        }

        PLTexture* texture = plCreateTexture();
        if(texture != NULL) {
            texture->filter = filter;
            if(plUploadTextureImage(texture, &img)) {
                return texture;
            }
        }
        plDestroyTexture(texture, true);
    }

    LogWarn("Failed to load texture, \"%s\" (%s)!\n", n_path, plGetError());
    plFreeImage(&img);
    return default_texture;
}

/************************************************************/

bool Display_AppendVideoPreset(int width, int height) {
    if(num_vid_presets >= MAX_VIDEO_PRESETS){
        LogWarn("Cannot append video preset, hit max limit! Try increasing \"MAX_VIDEO_PRESETS\"");
        return false;
    }
    vid_presets[num_vid_presets].width = width;
    vid_presets[num_vid_presets].height = height;
    num_vid_presets++;
    LogInfo("Added %dx%d video preset", width, height);
    return true;
}

void Display_ClearVideoPresets() {
    memset(vid_presets, 0, sizeof(VideoPreset) * MAX_VIDEO_PRESETS);
    num_vid_presets = 0;
    LogInfo("Cleared all video presets");
}

int Display_GetNumVideoPresets() {
    return num_vid_presets;
}

const VideoPreset* Display_GetVideoPreset(int idx) {
    if(idx < 0 || idx >= num_vid_presets) {
        LogWarn("Attempted to get an out of range video preset \'%d\', current count is %d", idx, num_vid_presets);
        return NULL;
    }
    return &vid_presets[idx];
}

/************************************************************/

/* shared function */
void Display_UpdateViewport(int x, int y, int width, int height) {
    Input_ResetStates();

    if(g_state.camera == NULL || g_state.ui_camera == NULL) {
        // display probably hasn't been initialised
        return;
    }

    float current_aspect = (float)width / (float)height;
    float target_aspect = 4.0f / 3.0f;
    float relative_width = target_aspect / current_aspect;
    float relative_height = current_aspect / target_aspect;
    relative_width = relative_width > 1.0f ? 1.0f : relative_width;
    relative_height = relative_height > 1.0f ? 1.0f : relative_height;

    int newWidth = (float)width * relative_width;
    int newHeight = (float)height * relative_height;

    //TODO: Only adjust viewport aspect of ingame camera once ingame scene is working. Force UI camera to 4:3 viewport always.
    //      For now, just use the same viewport aspect for both.

    if(FrontEnd_GetState() == FE_MODE_GAME || cv_display_use_window_aspect->b_value){
        //If enabled, use full window for 3d scene
        g_state.camera->viewport.x = g_state.ui_camera->viewport.x = x;
        g_state.camera->viewport.y = g_state.ui_camera->viewport.y = y;
        g_state.camera->viewport.w = g_state.ui_camera->viewport.w = width;
        g_state.camera->viewport.h = g_state.ui_camera->viewport.h = height;
    } else {
        g_state.camera->viewport.x = g_state.ui_camera->viewport.x = (width - newWidth) / 2;
        g_state.camera->viewport.y = g_state.ui_camera->viewport.y = (height - newHeight) / 2;
        g_state.camera->viewport.w = g_state.ui_camera->viewport.w = newWidth;
        g_state.camera->viewport.h = g_state.ui_camera->viewport.h = newHeight;
    }
}

int Display_GetViewportWidth(const PLViewport *viewport) {
    return viewport->w;
}

int Display_GetViewportHeight(const PLViewport *viewport) {
    return viewport->h;
}

/**
 * Update display to match the current settings.
 */
void Display_UpdateState(void) {
    char buf[4];

    // bound check both the width and height to lowest supported width and height
    if(cv_display_width->i_value < MIN_DISPLAY_WIDTH) {
        plSetConsoleVariable(cv_display_width, pl_itoa(MIN_DISPLAY_WIDTH, buf, 4, 10));
    }
    if(cv_display_height->i_value < MIN_DISPLAY_HEIGHT) {
        plSetConsoleVariable(cv_display_height, pl_itoa(MIN_DISPLAY_HEIGHT, buf, 4, 10));
    }

    // attempt to set the new display size, otherwise update cvars to match
    int w = cv_display_width->i_value;
    int h = cv_display_height->i_value;
    if(!System_SetWindowSize(w, h, false)) {
        LogWarn("failed to set display size to %dx%d!\n", cv_display_width->i_value, cv_display_height->i_value);
        LogInfo("display set to %dx%d\n", w, h);

        // ... not sure if we'll force the engine to only render a specific display size ...
        if(w < MIN_DISPLAY_WIDTH) w = MIN_DISPLAY_WIDTH;
        if(h < MIN_DISPLAY_HEIGHT) h = MIN_DISPLAY_HEIGHT;

        plSetConsoleVariable(cv_display_width, pl_itoa(w, buf, 4, 10));
        plSetConsoleVariable(cv_display_height, pl_itoa(h, buf, 4, 10));
    } else {
        LogInfo("display set to %dx%d\n", w, h);
    }

    Display_UpdateViewport(0, 0, cv_display_width->i_value, cv_display_height->i_value);
}

void Display_Initialize(void) {
    // check the command line for any arguments

    const char *var;
    if((var = plGetCommandLineArgumentValue("-width")) != NULL) {
        plSetConsoleVariable(cv_display_width, var);
    }
    if((var = plGetCommandLineArgumentValue("-height")) != NULL) {
        plSetConsoleVariable(cv_display_height, var);
    }

    if(plHasCommandLineArgument("-window")) {
        plSetConsoleVariable(cv_display_fullscreen, "false");
    } else if(plHasCommandLineArgument("-fullscreen")) {
        plSetConsoleVariable(cv_display_fullscreen, "true");
    }

    // now create the window and update the display
    System_DisplayWindow(false, MIN_DISPLAY_WIDTH, MIN_DISPLAY_HEIGHT);

    char win_title[32];
    snprintf(win_title, sizeof(win_title), ENGINE_TITLE " (%s)", GetVersionString());
    System_SetWindowTitle(win_title);

    Display_UpdateState();

    // platform library graphics subsystem can init now...
    plInitializeSubSystems(PL_SUBSYSTEM_GRAPHICS);
    plSetGraphicsMode(PL_GFX_MODE_OPENGL);

    //Create the render textures
    game_target = plCreateFrameBuffer(640, 480, PL_BUFFER_COLOUR | PL_BUFFER_DEPTH);
    if(game_target == NULL) {
        Error("Failed to create game_target framebuffer, aborting!\n%s\n", plGetError());
    }
    frontend_target = plCreateFrameBuffer(640, 480, PL_BUFFER_COLOUR | PL_BUFFER_DEPTH);
    if(frontend_target == NULL) {
        Error("Failed to create frontend_target framebuffer, aborting!\n%s\n", plGetError());
    }

    Shaders_Initialize();

    //////////////////////////////////////////////////////////

    plSetClearColour(PLColour(200, 200, 255, 255));

    g_state.camera = plCreateCamera();
    if(g_state.camera == NULL) {
        Error("failed to create camera, aborting!\n%s\n", plGetError());
    }
    g_state.camera->mode        = PL_CAMERA_MODE_PERSPECTIVE;
    g_state.camera->fov         = cv_camera_fov->f_value;
    g_state.camera->far         = cv_camera_far->f_value;
    g_state.camera->near        = cv_camera_near->f_value;
    g_state.camera->viewport.w  = cv_display_width->i_value;
    g_state.camera->viewport.h  = cv_display_height->i_value;

    g_state.ui_camera = plCreateCamera();
    if(g_state.ui_camera == NULL) {
        Error("failed to create ui camera, aborting!\n%s\n", plGetError());
    }
    g_state.ui_camera->mode         = PL_CAMERA_MODE_ORTHOGRAPHIC;
    g_state.ui_camera->fov          = 90;
    g_state.ui_camera->near         = 0;
    g_state.ui_camera->far          = 1000;
    g_state.ui_camera->viewport.w   = cv_display_width->i_value;
    g_state.ui_camera->viewport.h   = cv_display_height->i_value;

    ImGuiImpl_SetupCamera();

    plSetCullMode(PL_CULL_POSTIVE);

    plSetDepthMask(true);

    /* go ahead and create our placeholder texture, used if
     * one fails to load */

    PLColour pbuffer[]={{ 255, 255, 0  , 255 }, { 0  , 255, 255, 255 }, { 0  , 255, 255, 255 }, { 255, 255, 0  , 255 }};
    PLImage* image = plNewImage((uint8_t *)pbuffer, 2, 2, PL_COLOURFORMAT_RGBA, PL_IMAGEFORMAT_RGBA8);
    if(image != NULL) {
        default_texture = plCreateTexture();
        default_texture->flags &= PL_TEXTURE_FLAG_NOMIPS;
        if(!plUploadTextureImage(default_texture, image)) {
            LogWarn("Failed to upload default texture (%s)!\n", plGetError());
            plDestroyTexture(default_texture, true);
            default_texture = NULL;
        }
        plFreeImage(image);
    } else {
        LogWarn("Failed to generate default texture (%s)!\n", plGetError());
    }

    /* initialize the texture cache */

    LogInfo("Caching texture groups...\n");

    for(unsigned int i = 0; i < MAX_TEXTURE_INDEX; ++i) {
        Display_ClearTextureIndex(i);
    }

    Display_CacheTextureIndex("/chars/american/", "american.index", TEXTURE_INDEX_AMERICAN);
    Display_CacheTextureIndex("/chars/british/", "british.index", TEXTURE_INDEX_BRITISH);
    Display_CacheTextureIndex("/chars/french/", "french.index", TEXTURE_INDEX_FRENCH);
    Display_CacheTextureIndex("/chars/german/", "german.index", TEXTURE_INDEX_GERMAN);
    Display_CacheTextureIndex("/chars/japanese/", "japanese.index", TEXTURE_INDEX_JAPANESE);
    Display_CacheTextureIndex("/chars/russian/", "russian.index", TEXTURE_INDEX_RUSSIAN);
    Display_CacheTextureIndex("/chars/teamlard/", "teamlard.index", TEXTURE_INDEX_TEAMLARD);

    Display_CacheTextureIndex("/chars/weapons/", "weapons.index", TEXTURE_INDEX_WEAPONS);

    PrintTextureCacheSizeCommand(2, (char*[]){"", "MB"});
}

void Display_Shutdown(void) {
    Shaders_Shutdown();

    plDestroyTexture(default_texture, true);
}

/************************************************************/

void DEBUGDrawSkeleton();
void DEBUGDrawModel(void);

void Display_GetFramesCount(unsigned int *fps, unsigned int *ms) {
    static unsigned int fps_ = 0;
    static unsigned int ms_ = 0;
    static unsigned int update_delay = 60;
    if (update_delay < g_state.draw_ticks && g_state.last_draw_ms > 0) {
        ms_ = g_state.last_draw_ms;
        fps_ = 1000 / ms_;
        update_delay = g_state.draw_ticks + 60;
    }

    *fps = fps_;
    *ms = ms_;
}

static void DrawDisplayInfo(void) {
    char debug_display[128];
    int w, h;
    bool fs;
    System_GetWindowDrawableSize(&w, &h, &fs);
    sprintf(debug_display, "%d X %d %s", w, h, fs == true ? "FULLSCREEN" : "WINDOWED");
    Font_DrawBitmapString(g_fonts[FONT_GAME_CHARS], 20,
                          Display_GetViewportHeight(&g_state.ui_camera->viewport) - 32, 0, 1.f, PL_COLOUR_WHITE,
                          debug_display);
}

static void DrawFPSOverlay(void) {
    if(!cv_debug_fps->b_value) {
        return;
    }

    unsigned int fps, ms;
    Display_GetFramesCount(&fps, &ms);

    int w = Display_GetViewportWidth(&g_state.ui_camera->viewport);
    int h = Display_GetViewportHeight(&g_state.ui_camera->viewport);

    BitmapFont* font = g_fonts[FONT_SMALL];

    char ms_count[32];
    sprintf(ms_count, "FPS %d/S (%d/MS)", fps, ms);
    unsigned int str_w = (font->chars[0].w * 2) * strlen(ms_count);

    PLColour colour;
    if(fps < 20) {
        colour = PL_COLOUR_RED;
    } else if(fps < 30) {
        colour = PL_COLOUR_YELLOW;
    } else {
        colour = PL_COLOUR_GREEN;
    }

    Font_DrawBitmapString(font, w - str_w, h - (font->chars[0].h * 2), 0, 1.f, colour, ms_count);
}

static void DrawCameraInfoOverlay(void) {
    Font_DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "CAMERA");
    unsigned int y = 50;
    char cam_pos[32];
    snprintf(cam_pos, sizeof(cam_pos), "POSITION : %s", plPrintVector3(g_state.camera->position));
    Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y, 0, 1.f, PL_COLOUR_WHITE, cam_pos);
    snprintf(cam_pos, sizeof(cam_pos), "ANGLES   : %s", plPrintVector3(g_state.camera->angles));
    Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, cam_pos);
}

static void DrawDebugOverlay(void) {
    if(cv_debug_mode->i_value <= 0) {
        return;
    }

    UI_DisplayDebugMenu(); /* aka imgui */

    if(FrontEnd_GetState() == FE_MODE_INIT || FrontEnd_GetState() == FE_MODE_LOADING || cv_debug_mode->i_value <= 0) {
        return;
    }

    DrawDisplayInfo();
    //DrawCameraInfoOverlay();

    Font_DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "DRAW STATS");
    unsigned int y = 50;
    char cam_pos[32];
    snprintf(cam_pos, sizeof(cam_pos), "CHUNKS DRAWN : %d", g_state.gfx.num_chunks_drawn);
    Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y, 0, 1.f, PL_COLOUR_WHITE, cam_pos);
    snprintf(cam_pos, sizeof(cam_pos), "ACTORS DRAWN : %d", g_state.gfx.num_actors_drawn);
    Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, cam_pos);

    if (cv_debug_input->i_value > 0) {
        switch (cv_debug_input->i_value) {
            default: {
                Font_DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "KEYBOARD STATE");
                int x = 20, y = 50;
                for (unsigned int i = 0; i < INPUT_MAX_KEYS; ++i) {
                    bool status = Input_GetKeyState(i);
                    char key_state[64];
                    snprintf(key_state, sizeof(key_state), "%d (%s)", i, status ? "TRUE" : "FALSE");
                    Font_DrawBitmapString(g_fonts[FONT_SMALL], x, y, 0, 1.f, PL_COLOUR_WHITE, key_state);
                    if (y + 15 > Display_GetViewportHeight(&g_state.ui_camera->viewport) - 50) {
                        x += 90;
                        y = 50;
                    } else {
                        y += 15;
                    }
                }
            }
                break;

            case 2: {
                Font_DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "CONTROLLER STATE");

                char button_state[64];
                snprintf(button_state, sizeof(button_state), CHAR_PSX_CROSS " (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_CROSS) ? "TRUE" : "FALSE");
                unsigned int y = 50;
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_TRIANGLE " (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_TRIANGLE) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_CIRCLE " (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_CIRCLE) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_SQUARE " (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_SQUARE) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_L1 " (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_L1) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_L2 " (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_L2) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_R1 " (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_R1) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_R2 " (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_R2) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "START (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_START) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "SELECT (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_SELECT) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "UP (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_UP) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "DOWN (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_DOWN) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "LEFT (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_LEFT) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);

                snprintf(button_state, sizeof(button_state), "RIGHT (%s)",
                         Input_GetButtonState(0, PORK_BUTTON_RIGHT) ? "TRUE" : "FALSE");
                Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state);
            } break;
        }
        return;
    }
}

double cur_delta = 0;

void Display_SetupDraw(double delta) {
    cur_delta = delta;
    g_state.draw_ticks = System_GetTicks();

    unsigned int clear_flags = PL_BUFFER_DEPTH;
    if(FrontEnd_GetState() == FE_MODE_GAME || cv_debug_mode->i_value > 0) {
        clear_flags |= PL_BUFFER_COLOUR;
    }
    plClearBuffers(clear_flags);
    plSetDepthBufferMode(PL_DEPTHBUFFER_ENABLE);

    plBindFrameBuffer(NULL, PL_FRAMEBUFFER_DRAW);

    /* update camera state to match vars */
    g_state.camera->fov     = cv_camera_fov->f_value;
    g_state.camera->near    = cv_camera_near->f_value;
    g_state.camera->far     = cv_camera_far->f_value;

    plSetupCamera(g_state.camera);
}

void DrawActors(void); /* declared in engine.cpp */
void DrawMap(void); /* declared in engine.cpp */
void Display_DrawScene(void) {
    plSetShaderProgram(programs[SHADER_DEFAULT]);

    DrawMap();
    DrawActors();
    DrawParticles(cur_delta);

    DEBUGDrawModel();
}

void Display_DrawInterface(void) {
    plSetShaderProgram(programs[SHADER_DEFAULT]);
    plSetupCamera(g_state.ui_camera);
    plSetDepthBufferMode(PL_DEPTHBUFFER_DISABLE);
    FE_Draw();
}

void Display_DrawDebug(void) {
    int window_draw_w, window_draw_h;
    bool fs;
    System_GetWindowDrawableSize(&window_draw_w, &window_draw_h, &fs);

    plSetShaderProgram(programs[SHADER_DEFAULT]);
    plSetupCamera(g_state.ui_camera);

    DrawDebugOverlay();
    DrawFPSOverlay();

    Console_Draw();
}

void Display_Flush(void) {
    System_SwapDisplay();
    g_state.last_draw_ms = System_GetTicks() - g_state.draw_ticks;
}
