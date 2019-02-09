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

#pragma once

#include <PL/platform_graphics.h>
#include <PL/platform_math.h>
#include <PL/platform_console.h>
#include <PL/platform_model.h>

#include "util.h"
#include "pork_math.h"
#include "console.h"
#include "player.h"

#define ENGINE_TITLE        "OpenHoW"
#define ENGINE_APP_NAME     "OpenHoW"
#define ENGINE_LOG          "debug"

#define ENGINE_MAJOR_VERSION    0
#define ENGINE_MINOR_VERSION    1
#define ENGINE_PATCH_VERSION    0

static inline const char *GetVersionString(void) {
    /* this is here for when we start generating additional
     * versioning information (such as git version) */
    static char version[12] = { '\0' };
    if(version[0] == '\0') {
        snprintf(version, sizeof(version), "%d.%d.%d",
                 ENGINE_MAJOR_VERSION,
                 ENGINE_MINOR_VERSION,
                 ENGINE_PATCH_VERSION);
    }
    return &version[0];
}

typedef struct EngineState {
    struct PLCamera *camera;       // camera used for general gameplay
    struct PLCamera *ui_camera;    // camera used for UI elements, orthographic

    /* server / client logic */
    bool is_host;

    unsigned int sim_ticks;
    unsigned int last_sim_tick;

    unsigned int draw_ticks;
    unsigned int last_draw_ms;

    uint8_t max_players;            /* maximum players allowed in current game */
    Player players[MAX_PLAYERS];    /* tracks all current players */
} EngineState;
extern EngineState g_state;

#define GetUIViewportWidth()    640
#define GetUIViewportHeight()   480

PL_EXTERN_C

void Engine_Initialize(void);
void Engine_Shutdown(void);

void Engine_Simulate(void);

bool Engine_IsRunning(void);

const char *GetBasePath(void);
const char *GetCampaignPath(void);

/************************************************************/

enum LogLevel {
    LOG_LEVEL_DEFAULT,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_DEBUG,
};

enum PromptLevel {
    PROMPT_LEVEL_DEFAULT,
    PROMPT_LEVEL_WARNING,
    PROMPT_LEVEL_ERROR,
};

typedef unsigned int uint;
typedef unsigned char uchar, byte;

#define _print_w_function(LEVEL, FORMAT, ...) plLogMessage((LEVEL), "(%s) " FORMAT, PL_FUNCTION, ## __VA_ARGS__)

#ifdef _DEBUG
#   define LogDebug(...) _print_w_function(LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#   define LogDebug(...) ()
#endif

/************************************************************/
/* System */

unsigned int System_GetTicks(void);

void System_DisplayMessageBox(unsigned int level, const char *msg, ...);
void System_DisplayWindow(bool fullscreen, int width, int height);

void System_SwapDisplay(void);

void System_SetWindowTitle(const char *title);
bool System_SetWindowSize(int *width, int *height, bool fs);

void System_Shutdown(void);

PL_EXTERN_C_END

#define LogInfo(...)    _print_w_function(LOG_LEVEL_DEFAULT, __VA_ARGS__)
#define LogWarn(...)    _print_w_function(LOG_LEVEL_WARNING, __VA_ARGS__)
#define Error(...) {                                            \
    _print_w_function(LOG_LEVEL_ERROR, __VA_ARGS__);      \
    System_DisplayMessageBox(PROMPT_LEVEL_ERROR, __VA_ARGS__); \
    exit(EXIT_FAILURE);                                         \
}
