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

#include "engine.h"
#include "input.h"
#include "language.h"

#include "client/particle.h"
#include "client/display.h"
#include "client/font.h"
#include "client/frontend.h"

/************************************************************/

#define check_args(num) if(argc < (num)) { LogWarn("invalid number of arguments (%d < %d), ignoring!\n", argc, (num)); return; }

typedef struct CallbackConstruct {
    const char *cmd;

    void(*Get)(unsigned int argc, char *argv[]);
    void(*Set)(unsigned int argc, char *argv[]);
    void(*Add)(unsigned int argc, char *argv[]);
} CallbackConstruct;

static CallbackConstruct callbacks[]={
        {"menu", NULL, SetFEObjectCommand, NULL},
        {"actor", NULL, NULL, NULL},
        {"map", NULL, NULL, NULL},
};

static void GetCommand(unsigned int argc, char *argv[]) {
    check_args(2);

    const char *cmd = argv[1];
    for(unsigned int j = 0; j < plArrayElements(callbacks); ++j) {
        if(pl_strcasecmp(callbacks[j].cmd, cmd) == 0) {
            if(callbacks[j].Get == NULL) {
                break;
            }
            callbacks[j].Get(argc, argv);
            return;
        }
    }
    LogWarn("invalid GET command, %s!\n", cmd);
}

static void AddCommand(unsigned int argc, char *argv[]) {
    check_args(2);

    const char *cmd = argv[1];
    for(unsigned int j = 0; j < plArrayElements(callbacks); ++j) {
        if(pl_strcasecmp(callbacks[j].cmd, cmd) == 0) {
            if(callbacks[j].Add == NULL) {
                break;
            }
            callbacks[j].Add(argc, argv);
            return;
        }
    }
    LogWarn("invalid ADD command, %s!\n", cmd);
}

static void SetCommand(unsigned int argc, char *argv[]) {
    check_args(2);

    const char *cmd = argv[1];
    for(unsigned int j = 0; j < plArrayElements(callbacks); ++j) {
        if(pl_strcasecmp(callbacks[j].cmd, cmd) == 0) {
            if(callbacks[j].Add == NULL) {
                break;
            }
            callbacks[j].Add(argc, argv);
            return;
        }
    }
    LogWarn("invalid SET command, %s!\n", cmd);

#if 0
    for(unsigned int i = 1; i < argc; ++i) {
        if(pl_strncasecmp("actor", argv[i], 5) == 0) {
            const char *actor_name = argv[++i];
            if(actor_name == NULL || actor_name[0] == '\0') {
                LogWarn("invalid actor name, aborting\n");
                return;
            }

            if(g_state.is_host) {
                Actor *actor = SVGetActor(actor_name);
                if (actor == NULL) {
                    LogWarn("unable to find actor %s, aborting\n", actor_name);
                    return;
                }

                const char *modifier = argv[++i];
                if (modifier == NULL || modifier[0] == '\0') {
                    LogWarn("no modifier provided for actor %s, aborting\n", actor_name);
                    return;
                }

                /* todo, add table of string + parm so we don't need to hard-code most of this */
                if (pl_strncasecmp("name", modifier, 4) == 0) {
                    const char *name = argv[++i];
                    if (name == NULL || name[0] == '\0') {
                        LogWarn("invalid name, aborting\n");
                        return;
                    }

                    strncpy(actor->name, name, sizeof(actor->name));
                    return;
                } else if (pl_strncasecmp("position", modifier, 8) == 0) {

                } else if (pl_strncasecmp("bounds", modifier, 6) == 0) {

                }
            }
            return;
        } else if(pl_strncasecmp(CMD_GROUP_MAP, argv[i], 3) == 0) {
            /* todo, set map properties */
            const char *name = argv[++i];
            if(name == NULL || name[0] == '\0') {
                LogWarn("invalid map name, aborting\n");
                return;
            }

            LoadMap(name, MAP_MODE_DEATHMATCH);
            return;
        }
    }

    LogWarn("invalid parameters provided for set command\n");
#endif
}

static void FrontendModeCommand(unsigned int argc, char *argv[]) {
    check_args(2);

    int mode = atoi(argv[1]);
    if(mode < 0) {
        LogWarn("invalid frontend mode, \"%d\", specified!\n", mode);
        return;
    }

    FE_SetState((unsigned int) mode);
}

static void UpdateDisplayCommand(unsigned int argc, char *argv[]) {
    Display_UpdateState();
}

static void QuitCommand(unsigned int argc, char *argv[]) {
    System_Shutdown();
}

static void DisconnectCommand(unsigned int argc, char *argv[]) {
    //Map_Unload();
}

static void ConfigCommand(unsigned int argc, char *argv[]) {
    check_args(2);

    void Config_Load(const char *path);
    Config_Load(argv[1]);
}

static void OpenCommand(unsigned int argc, char *argv[]) {
    check_args(2);

    const char *fpath = argv[1];
    if(plIsEmptyString(fpath)) {
        LogWarn("invalid argument provided, ignoring!\n");
        return;
    }

    enum {
        TYPE_UNKNOWN,
        TYPE_MODEL,
        TYPE_PARTICLE,
        TYPE_MAP,
        TYPE_SOUND,

        MAX_TYPES
    };
    unsigned int type = TYPE_UNKNOWN;

    /* now we just need to figure out what kind of file it is */
    const char *ext = plGetFileExtension(fpath);
    if(!plIsEmptyString(ext)) {
        if(pl_strncasecmp(ext, "vtx", 3) == 0 ||
           pl_strncasecmp(ext, "fac", 3) == 0 ||
           pl_strncasecmp(ext, "no2", 3) == 0) {
            type = TYPE_MODEL;
        } else if(pl_strncasecmp(ext, "pmg", 3) == 0 ||
                  pl_strncasecmp(ext, "pog", 3) == 0 ||
                  pl_strncasecmp(ext, "map", 3) == 0) {
            type = TYPE_MAP;
        } else if(pl_strncasecmp(ext, PPS_EXTENSION, 3) == 0) {
            type = TYPE_PARTICLE;
        }
    }

    switch(type) {
        default: {
            LogWarn("unknown filetype, ignoring!\n");
        } break;

        case TYPE_MAP: {
            char map_name[32];
            snprintf(map_name, sizeof(map_name), "%s", plGetFileName(fpath) - 4);
            if(plIsEmptyString(map_name)) {
                LogWarn("invalid map name, ignoring!\n");
                return;
            }

            u_assert(0);
            //Map_Load(map_name, MAP_MODE_EDITOR);
        } break;
    }
}

static void DebugModeCallback(const PLConsoleVariable *variable) {
    plSetupLogLevel(LOG_LEVEL_DEBUG, "debug", PLColour(0, 255, 255, 255), variable->b_value);
}

/************************************************************/

#define MAX_BUFFER_SIZE 512

struct {
    char buffer[MAX_BUFFER_SIZE];
    unsigned int buffer_pos;
} console_state;

static bool console_enabled = false;

PLConsoleVariable *cv_debug_mode = NULL;
PLConsoleVariable *cv_debug_fps = NULL;
PLConsoleVariable *cv_debug_skeleton = NULL;
PLConsoleVariable *cv_debug_input = NULL;
PLConsoleVariable *cv_debug_cache = NULL;

PLConsoleVariable *cv_camera_mode = NULL;

PLConsoleVariable *cv_base_path = NULL;
PLConsoleVariable *cv_campaign_path = NULL;

PLConsoleVariable *cv_display_texture_cache = NULL;
PLConsoleVariable *cv_display_width = NULL;
PLConsoleVariable *cv_display_height = NULL;
PLConsoleVariable *cv_display_fullscreen = NULL;
PLConsoleVariable *cv_display_use_window_aspect = NULL;
PLConsoleVariable *cv_display_ui_scale = NULL;

PLConsoleVariable *cv_audio_volume = NULL;
PLConsoleVariable *cv_audio_volume_sfx = NULL;
PLConsoleVariable *cv_audio_voices = NULL;
PLConsoleVariable *cv_audio_mode = NULL;

void Console_Initialize(void) {
#define rvar(var, arc, ...) \
    { \
        const char *str_##var = plStringify(var); \
        (var) = plRegisterConsoleVariable(&str_##var[3], __VA_ARGS__); \
        (var)->archive = (arc); \
    }

    rvar(cv_debug_mode, false, "1", pl_int_var, DebugModeCallback, "global debug level");
    rvar(cv_debug_fps, false, "1", pl_bool_var, NULL, "display framerate");
    rvar(cv_debug_skeleton, false, "0", pl_bool_var, NULL, "display pig skeletons");
    rvar(cv_debug_input, false, "0", pl_int_var, NULL,
                          "changing this cycles between different modes of debugging input\n"
                          "1: keyboard states\n2: controller states"
    );
    rvar(cv_debug_cache, false, "0", pl_bool_var, NULL, "display memory and other info");

    rvar(cv_base_path, true, ".", pl_string_var, NULL, "");
    rvar(cv_campaign_path, false, "", pl_string_var, NULL, "");

    rvar(cv_camera_mode, false, "0", pl_int_var, NULL, "0 = default, 1 = debug");

    rvar(cv_display_texture_cache, false, "-1", pl_int_var, NULL, "");
    rvar(cv_display_width, true, "1024", pl_int_var, NULL, "");
    rvar(cv_display_height, true, "768", pl_int_var, NULL, "");
    rvar(cv_display_fullscreen, true, "false", pl_bool_var, NULL, "");
    rvar(cv_display_use_window_aspect, false, "false", pl_bool_var, NULL, "");
    rvar(cv_display_ui_scale, true, "1", pl_int_var, NULL,"0 = automatic scale"
    );

    rvar(cv_audio_volume, true, "1", pl_float_var, NULL, "set global audio volume");
    rvar(cv_audio_volume_sfx, true, "1", pl_float_var, NULL, "set sfx audio volume");
    rvar(cv_audio_mode, true, "1", pl_int_var, NULL, "0 = mono, 1 = stereo");
    rvar(cv_audio_voices, true, "true", pl_bool_var, NULL, "enable/disable pig voices");

    plRegisterConsoleVariable("language", "eng", pl_string_var, SetLanguageCallback, "Current language");

    void ConvertImageCallback(unsigned int argc, char *argv[]);
    void PrintTextureCacheSizeCommand(unsigned int argc, char *argv[]);

    plRegisterConsoleCommand("printtcache", PrintTextureCacheSizeCommand, "displays current texture memory usage");
    plRegisterConsoleCommand("set", SetCommand, "Sets state for given target");
    plRegisterConsoleCommand("get", GetCommand, "Gets state for given target");
    plRegisterConsoleCommand("add", AddCommand, "Adds the given target");
    plRegisterConsoleCommand("open", OpenCommand, "Opens the specified file");
    plRegisterConsoleCommand("exit", QuitCommand, "Closes the game");
    plRegisterConsoleCommand("quit", QuitCommand, "Closes the game");
    plRegisterConsoleCommand("config", ConfigCommand, "Loads the specified config");
    plRegisterConsoleCommand("disconnect", DisconnectCommand, "Disconnects and unloads current map");
    plRegisterConsoleCommand("display_update", UpdateDisplayCommand, "Updates the display to match current settings");
    plRegisterConsoleCommand("femode", FrontendModeCommand, "Forcefully change the current mode for the frontend");
}

void Console_Draw(void) {
    if(FE_GetState() == FE_MODE_INIT || FE_GetState() == FE_MODE_LOADING || !console_enabled) {
        return;
    }

    plSetBlendMode(PL_BLEND_DEFAULT);

    plDrawFilledRectangle(plCreateRectangle(
            PLVector2(0, 0),
            PLVector2(Display_GetViewportWidth(&g_state.ui_camera->viewport), 32),
            PLColour(0, 0, 0, 255),
            PLColour(0, 0, 0, 0),
            PLColour(0, 0, 0, 255),
            PLColour(0, 0, 0, 0)
    ));

    char msg_buf[MAX_BUFFER_SIZE];
    strncpy(msg_buf, console_state.buffer, sizeof(msg_buf));
    pl_strtoupper(msg_buf);

    plSetBlendMode(PL_BLEND_ADDITIVE);

    unsigned int x = 20;
    unsigned int w = g_fonts[FONT_GAME_CHARS]->chars[0].w;
    Font_DrawBitmapCharacter(g_fonts[FONT_GAME_CHARS], x, 10, 1.f, PL_COLOUR_RED, '>');
    if(console_state.buffer_pos > 0) {
        Font_DrawBitmapString(g_fonts[FONT_GAME_CHARS], x + w + 5, 10, 1, 1.f, PL_COLOUR_WHITE, msg_buf);
    }

    /* DrawBitmapString disables blend - no need to call again here */

    plSetBlendMode(PL_BLEND_DEFAULT);
}

static void ResetConsole(void) {
    memset(console_state.buffer, 0, sizeof(console_state.buffer));
    console_state.buffer_pos = 0;
}

static void ConsoleInputCallback(int key, bool is_pressed) {
    if(!is_pressed || !console_enabled) {
        return;
    }

    if(key == '\r') {
        /* todo, allow multiple commands, seperated with ';' */
        plParseConsoleString(console_state.buffer);
        Console_Toggle();
        return;
    }

    if(key == '\b' && console_state.buffer_pos > 0) {
        console_state.buffer[--console_state.buffer_pos] = '\0';
        return;
    }

    if(console_state.buffer_pos > MAX_BUFFER_SIZE) {
        LogWarn("hit console buffer limit, aborting!\n");
        return;
    }

    if(isprint(key) == 0) {
        return;
    }

    console_state.buffer[console_state.buffer_pos++] = (char) key;
}

void Console_Toggle(void) {
    console_enabled = !console_enabled;
    if(console_enabled) {
        Input_SetKeyboardFocusCallback(ConsoleInputCallback);
        ResetConsole();
    } else {
        Input_SetKeyboardFocusCallback(NULL);
    }

    Input_ResetStates();
}
