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
#include "pork_engine.h"
#include "pork_input.h"
#include "pork_font.h"

#include "server/server_actor.h"
#include "pork_map.h"

/*****************************************************/

void GetCommand(unsigned int argc, char *argv[]) {
    if(argc < 1) {
        return;
    }

    for(unsigned int i = 1; i < argc; ++i) {

    }
}

void SetCommand(unsigned int argc, char *argv[]) {
    if(argc < 1) {
        return;
    }

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
        } else if(pl_strncasecmp("map", argv[i], 3) == 0) {
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
}

/*****************************************************/

#define MAX_BUFFER_SIZE 512

struct {
    char buffer[MAX_BUFFER_SIZE];
    unsigned int buffer_pos;
} console_state;

bool console_enabled = false;

bool IsConsoleEnabled(void) {
    return console_enabled;
}

void DrawConsole(void) {
    if(!console_enabled) {
        return;
    }

    char msg_buf[MAX_BUFFER_SIZE];
    strncpy(msg_buf, console_state.buffer, sizeof(msg_buf));
    pl_strtoupper(msg_buf);

    unsigned int x = 20;
    unsigned int w = g_fonts[FONT_CHARS2]->chars[0].w;
    DrawBitmapCharacter(g_fonts[FONT_CHARS2], x, 20, 1.f, '>');
    if(console_state.buffer_pos > 0) {
        DrawBitmapString(g_fonts[FONT_CHARS2], x + w + 5, 20, 1, 1.f, msg_buf);
    }
}

void ResetConsole(void) {
    memset(console_state.buffer, 0, sizeof(console_state.buffer));
    console_state.buffer_pos = 0;
}

void ConsoleInputCallback(int key, bool is_pressed) {
    if(!is_pressed) {
        return;
    }

    if(key == '\r') {
        plParseConsoleString(console_state.buffer);
        ResetConsole();
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

    console_state.buffer[console_state.buffer_pos++] = (char) key;
}

void ToggleConsole(void) {
    console_enabled = !console_enabled;
    if(console_enabled) {
        SetKeyboardFocusCallback(ConsoleInputCallback);
        ResetConsole();
    } else {
        SetKeyboardFocusCallback(NULL);
    }

    ResetInputStates();
}