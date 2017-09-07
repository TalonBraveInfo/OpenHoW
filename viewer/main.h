
#pragma once

#include <PL/platform_log.h>
#include <PL/platform_window.h>
#include <PL/platform_graphics.h>
#include <PL/platform_model.h>
#include <PL/platform_filesystem.h>

#define TITLE   "War Hogs"
#define LOG     "debug"

#define VERSION_MAJOR   0
#define VERSION_MINOR   0

#define PSX_WIDTH   320
#define PSX_HEIGHT  240

#define PRINT(...)          printf(__VA_ARGS__); plWriteLog(LOG, __VA_ARGS__)
#define PRINT_ERROR(...)    PRINT(__VA_ARGS__); exit(-1)
#ifdef _DEBUG
#   define DPRINT(...)      PRINT(__VA_ARGS__)
#else
#   define DPRINT(...)      (__VA_ARGS__)
#endif

typedef struct GlobalVars {
    bool is_psx, is_psx_mode;

    unsigned int width, height;

    PLCamera *main_camera;

    struct {
        unsigned int num_players;

        bool is_paused;
    } game;

    struct {
        unsigned int colour_rbo, depth_rbo;
        unsigned int psx_fbo;
    } gl;
} GlobalVars;

extern GlobalVars g_state;

///////////////////////////////////////////////////

#define MAX_PLAYERS 64

#define MAX_STATIC_OBJECTS  1024
