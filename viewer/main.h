
#pragma once

#include <platform_log.h>
#include <platform_window.h>
#include <platform_graphics.h>
#include <platform_model.h>
#include <platform_filesystem.h>

#define TITLE       "Piggy Viewer"
#define LOG         "hog_loader"
#define PRINT(...)  printf(__VA_ARGS__); plWriteLog(LOG, __VA_ARGS__)
#define PRINT_ERROR(...) PRINT(__VA_ARGS__); exit(-1)

typedef struct GlobalVars {
    bool is_psx;

    unsigned int width, height;
} GlobalVars;

extern GlobalVars g_state;
