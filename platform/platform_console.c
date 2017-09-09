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

#include "graphics/graphics_private.h"

#include <PL/platform_console.h>
#include <PL/platform_graphics_font.h>
#include <PL/platform_input.h>

/* Multi Console Manager */
// todo, should the console be case-sensitive?
// todo, mouse input callback
// todo, keyboard input callback

typedef struct PLConsoleCommand {
    char cmd[24];

    void(*Callback)(unsigned int argc, char *argv[]);

    char description[512];
} PLConsoleCommand;

PLConsoleCommand **_pl_commands = NULL;
size_t _pl_num_commands = 0;
size_t _pl_commands_size = 512;

#define IMPLEMENT_COMMAND(NAME, DESC) \
    void NAME ## _func(unsigned int argc, char *argv[]); \
    PLConsoleCommand NAME ## _var = {#NAME, NAME ## _func, DESC}; \
    void NAME ## _func(unsigned int argc, char *argv[])

void plRegisterConsoleCommands(PLConsoleCommand cmds[], unsigned int num_cmds) {

    // Deal with resizing the array dynamically...
    if((num_cmds + _pl_num_commands) > _pl_commands_size) {
        PLConsoleCommand **old_mem = _pl_commands;
        _pl_commands = (PLConsoleCommand**)realloc(_pl_commands, (_pl_commands_size += 128) * sizeof(PLConsoleCommand));
        if(!_pl_commands) {
            _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate %d bytes!\n",
                           _pl_commands_size * sizeof(PLConsoleCommand));
            _pl_commands = old_mem;
            _pl_commands_size -= 128;
            return;
        }
    }

    for(unsigned int i = 0; i < num_cmds; i++) {
        if((cmds[i].cmd[0] == '\0') || !(cmds[i].Callback)) {
            continue;
        }

        if(_pl_num_commands < _pl_commands_size) {
            _pl_commands[_pl_num_commands] = (PLConsoleCommand*)malloc(sizeof(PLConsoleCommand));
            if(!_pl_commands[_pl_num_commands]) {
                _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate memory for ConsoleCommand, %d!\n",
                               sizeof(PLConsoleCommand));
                break;
            }
            memcpy(_pl_commands[_pl_num_commands], &cmds[i], sizeof(PLConsoleCommand));
            _pl_num_commands++;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////

// todo, console variable implementation goes here!

PLConsoleVariable **_pl_variables = NULL;
size_t _pl_num_variables = 0;
size_t _pl_variables_size = 512;

void plRegisterConsoleVariables(PLConsoleVariable vars[], unsigned int num_vars) {
    plAssert(_pl_variables);

    // Deal with resizing the array dynamically...
    if((num_vars + _pl_num_variables) > _pl_variables_size) {
        PLConsoleVariable **old_mem = _pl_variables;
        _pl_variables = (PLConsoleVariable**)realloc(_pl_variables, (_pl_variables_size += 128) * sizeof(PLConsoleVariable));
        if(!_pl_variables) {
            _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate %d bytes!\n",
                           _pl_variables_size * sizeof(PLConsoleVariable));
            _pl_variables = old_mem;
            _pl_variables_size -= 128;
            return;
        }
    }

    for(unsigned int i = 0; i < num_vars; i++) {
        if(vars[i].var[0] == '\0') {
            continue;
        }

        if(_pl_num_variables < _pl_variables_size) {
            _pl_variables[_pl_num_variables] = (PLConsoleVariable*)malloc(sizeof(PLConsoleVariable));
            if(!_pl_variables[_pl_num_variables]) {
                _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate memory for ConsoleCommand, %d!\n",
                               sizeof(PLConsoleVariable));
                break;
            }
            memcpy(_pl_variables[_pl_num_variables], &vars[i], sizeof(PLConsoleVariable));
            _pl_variables[_pl_num_variables]->default_value = _pl_variables[_pl_num_variables]->value;
            _pl_num_variables++;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////

#define PLCONSOLE_MAX_INSTANCES 4

#define PLCONSOLE_DEFAULT_COLOUR 128, 0, 0, 128

typedef struct PLConsolePane {
    PLRectangle display;

    char buffer[4096];
} PLConsolePane;

PLConsolePane _pl_console_pane[PLCONSOLE_MAX_INSTANCES];
unsigned int _pl_num_console_panes;
unsigned int _pl_active_console_pane = 0;

bool _pl_console_visible = false;

/////////////////////////////////////////////////////////////////////////////////////
// PRIVATE

IMPLEMENT_COMMAND(cls, "Clears the console buffer.") {

}

IMPLEMENT_COMMAND(colour, "Changes the colour of the current console.") {

}

IMPLEMENT_COMMAND(time, "Prints out the current time.") {

}

IMPLEMENT_COMMAND(mem, "Prints out current memory usage.") {

}

IMPLEMENT_COMMAND(cmds, "Produces list of existing commands.") {
    for(PLConsoleCommand **cmd = _pl_commands; cmd < _pl_commands + _pl_num_commands; ++cmd) {
        printf(" %-20s : %-20s\n", (*cmd)->cmd, (*cmd)->description);
    }
    printf("%zu commands in total\n", _pl_num_commands);
}

IMPLEMENT_COMMAND(vars, "Produces list of existing variables.") {
    for(PLConsoleVariable **var = _pl_variables; var < _pl_variables + _pl_num_variables; ++var) {
        printf(" %-20s : %-5s / %-15s : %-20s\n",
               (*var)->var, (*var)->value, (*var)->default_value, (*var)->description);
    }
    printf("%zu variables in total\n", _pl_num_variables);
}

IMPLEMENT_COMMAND(help, "") {
    if(argc > 1) { // looking for assistance on a command probably...

    }
}

//////////////////////////////////////////////

PLMesh *_pl_mesh_line = NULL;
PLBitmapFont *_pl_console_font = NULL;

PLresult _plInitConsole(void) {
    _pl_console_visible = false;

    if(!(_pl_console_font = plCreateBitmapFont("fonts/console.font"))) {
        // todo, print warning...
    }

    memset(&_pl_console_pane, 0, sizeof(PLConsolePane) * PLCONSOLE_MAX_INSTANCES);
    _pl_active_console_pane = _pl_num_console_panes = 0;

    if(!(_pl_mesh_line = plCreateMesh(PLMESH_LINES, PL_DRAW_IMMEDIATE, 0, 4))) {
        return PL_RESULT_MEMORYALLOC;
    }

    if(!(_pl_commands = (PLConsoleCommand**)malloc(sizeof(PLConsoleCommand*) * _pl_commands_size))) {
        _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate memory for ConsoleCommand array, %d!\n",
                       sizeof(PLConsoleCommand) * _pl_commands_size);
        return PL_RESULT_MEMORYALLOC;
    }

    if(!(_pl_variables = (PLConsoleVariable**)malloc(sizeof(PLConsoleVariable*) * _pl_variables_size))) {
        _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate memory for ConsoleVariable array, %d!\n",
                       sizeof(PLConsoleCommand) * _pl_commands_size);
        return PL_RESULT_MEMORYALLOC;
    }

    PLConsoleCommand base_commands[]={
            cls_var,
            help_var,
            time_var,
            mem_var,
            colour_var,
    };
    plRegisterConsoleCommands(base_commands, plArrayElements(base_commands));

    plAddConsoleVariable(MyVar, "true", PL_VAR_BOOLEAN, NULL, "Example console variable, that does nothing!");
    plAddConsoleVariable(YourVar, "false", PL_VAR_BOOLEAN, NULL, "Example console variable, that does nothing!");
    plAddConsoleVariable(HisVar, "apple", PL_VAR_BOOLEAN, NULL, "Example console variable, that does nothing!");

    // todo, temporary
    cmds_func(0, NULL);
    vars_func(0, NULL);

    // todo, parse config

    return PL_RESULT_SUCCESS;
}

void _plShutdownConsole(void) {
    _pl_console_visible = false;

    plDeleteBitmapFont(_pl_console_font);

    memset(&_pl_console_pane, 0, sizeof(PLConsolePane) * PLCONSOLE_MAX_INSTANCES);
    _pl_active_console_pane = _pl_num_console_panes = 0;

    if(_pl_commands) {
        for(PLConsoleCommand **cmd = _pl_commands; cmd < _pl_commands + _pl_num_commands; ++cmd) {
            // todo, should we return here; assume it's the end?
            if (!(*cmd)) {
                continue;
            }

            free((*cmd));
        }
        free(_pl_commands);
    }

    if(_pl_variables) {
        for(PLConsoleVariable **var = _pl_variables; var < _pl_variables + _pl_num_variables; ++var) {
            // todo, should we return here; assume it's the end?
            if (!(*var)) {
                continue;
            }

            free((*var));
        }
        free(_pl_commands);
    }
}

#define _MAX_ROWS       2
#define _MAX_COLUMNS    2

// todo, correctly handle rows and columns.
void _plResizeConsoles(void) {
    unsigned int screen_w = pl_graphics_state.viewport_width, screen_h = pl_graphics_state.viewport_height;
    if(screen_w == 0 || screen_h == 0) {
        screen_w = 640;
        screen_h = 480;
    }
    unsigned int width = screen_w; // / _pl_num_console_panes;
    if(_pl_num_console_panes > 1) {
        width = screen_w / 2;
    }
    unsigned int height = screen_h;
    if(_pl_num_console_panes > 2) {
        height = screen_h / 2;
    }
    unsigned int position_x = 0, position_y = 0;
    for(unsigned int i = 0; i < _pl_num_console_panes; i++) {
        if(i > 0) {
            position_x += width;
            if(position_x >= screen_w) {
                // Move onto the next row
                position_x = 0;
                position_y += height;
                if(position_y > screen_h) {
                    // Return back to the first row (this shouldn't occur...)
                    position_y = 0;
                }
            }
        }

        _pl_console_pane[i].display.wh.x = width;
        _pl_console_pane[i].display.wh.y = height;
        _pl_console_pane[i].display.xy.x = position_x;
        _pl_console_pane[i].display.xy.x = position_y;
    }
}

bool _plIsConsolePaneVisible(unsigned int id) {
    if(!_pl_console_visible) {
        return false;
    }

    if(
        _pl_console_pane[id].display.ll.a == 0 &&
        _pl_console_pane[id].display.lr.a == 0 &&
        _pl_console_pane[id].display.ul.a == 0 &&
        _pl_console_pane[id].display.ur.a == 0 ) {
        return false;
    }

    return true;
}

// INPUT

void _plConsoleInput(int m_x, int m_y, unsigned int m_buttons, bool is_pressed) {
    if(!is_pressed) {
        return;
    }

    for(unsigned int i = 0; i < _pl_num_console_panes; i++) {
        if(!_plIsConsolePaneVisible(i)) {
            continue;
        }

        PLConsolePane *pane = &_pl_console_pane[i];

        int pane_min_x = (int)pane->display.xy.x;
        int pane_max_x = pane_min_x + (int)(pane->display.wh.x);
        if(m_x < pane_min_x || m_x > pane_max_x) {
            continue;
        }

        int pane_min_y = (int)pane->display.xy.y;
        int pane_max_y = pane_min_y + (int)(pane->display.wh.y);
        if(m_y < pane_min_y || m_y > pane_max_y) {
            continue;
        }

        if(m_buttons & PLINPUT_MOUSE_LEFT) {
            _pl_active_console_pane = i;

            pane->display.xy.x += m_x; pane->display.xy.y += m_y;
            if(pane->display.xy.x <= pl_graphics_state.viewport_x) {
                pane->display.xy.x = pl_graphics_state.viewport_x + 1;
            } else if(pane->display.xy.x >= pl_graphics_state.viewport_width) {
                pane->display.xy.x = pl_graphics_state.viewport_width - 1;
            } else if(pane->display.xy.y <= pl_graphics_state.viewport_y) {
                pane->display.xy.y = pl_graphics_state.viewport_y + 1;
            } else if(pane->display.xy.y >= pl_graphics_state.viewport_height) {
                pane->display.xy.y = pl_graphics_state.viewport_height - 1;
            }

            static int old_x = 0, old_y = 0;

            return;
        }

        if(m_buttons & PLINPUT_MOUSE_RIGHT) {
        // todo, display context menu
            return;
        }

        return;
    }

    // If we reached here, then we failed to hit anything...

    if(m_buttons & PLINPUT_MOUSE_RIGHT) {
    // todo, display context menu
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// PUBLIC

// GENERAL

void plSetupConsole(unsigned int num_instances) {
    if(num_instances == 0) {
        return;
    }

    if(num_instances > PLCONSOLE_MAX_INSTANCES) {
        num_instances = PLCONSOLE_MAX_INSTANCES;
    }

    memset(&_pl_console_pane, 0, sizeof(PLConsolePane) * num_instances);

    for(unsigned int i = 0; i < num_instances; i++) {
        plSetRectangleUniformColour(&_pl_console_pane[i].display, plCreateColour4b(PLCONSOLE_DEFAULT_COLOUR));
    }

    _pl_num_console_panes = num_instances;
    _plResizeConsoles();
}

void plShowConsole(bool show) {
    _pl_console_visible = show;
}

void plSetConsoleColour(unsigned int id, PLColour colour) {
    plSetRectangleUniformColour(&_pl_console_pane[id-1].display, colour);
}

// todo, hook with log output?
// todo, multiple warning/error levels?
// todo, correctly adjust buffer.
void plPrintConsoleMessage(unsigned int id, const char *msg, ...) {
    if(id > PLCONSOLE_MAX_INSTANCES) {
        return;
    }

    char buf[2048] = { 0 };

    va_list args;
    va_start(args, msg);
    vsprintf(buf, msg, args);
    va_end(args);

    strncat(_pl_console_pane[id].buffer, buf, strlen(buf));
}

#define _COLOUR_INACTIVE_ALPHA_TOP      128
#define _COLOUR_INACTIVE_ALPHA_BOTTOM   80

#define _COLOUR_HEADER_INACTIVE         20, 0, 0, _COLOUR_INACTIVE_ALPHA_TOP

#define _COLOUR_ACTIVE_ALPHA_TOP        255
#define _COLOUR_ACTIVE_ALPHA_BOTTOM     128

#define _COLOUR_HEADER_ACTIVE_TOP       128, 0, 0, _COLOUR_ACTIVE_ALPHA_TOP
#define _COLOUR_HEADER_ACTIVE_BOTTOM    82, 0, 0, _COLOUR_ACTIVE_ALPHA_BOTTOM

void plDrawConsole(void) {
    _plResizeConsoles();

    for(unsigned int i = 0; i < _pl_num_console_panes; i++) {
        if(!_plIsConsolePaneVisible(i)) {
            continue;
        }

        if(i == _pl_active_console_pane) {
            plDrawRectangle(_pl_console_pane[i].display);

            plDrawRectangle(plCreateRectangle(

                    plCreateVector2D(
                            _pl_console_pane[i].display.xy.x + 4,
                            _pl_console_pane[i].display.xy.y + 4
                    ),

                    plCreateVector2D(
                            _pl_console_pane[i].display.wh.x - 8,
                            20
                    ),

                    plCreateColour4b(_COLOUR_HEADER_ACTIVE_TOP),
                    plCreateColour4b(_COLOUR_HEADER_ACTIVE_TOP),
                    plCreateColour4b(_COLOUR_HEADER_ACTIVE_BOTTOM),
                    plCreateColour4b(_COLOUR_HEADER_ACTIVE_BOTTOM)
            ));

            // todo, console title
            // todo, display scroll bar
        } else {
            plDrawRectangle(plCreateRectangle(

                    plCreateVector2D(
                            _pl_console_pane[i].display.xy.x,
                            _pl_console_pane[i].display.xy.y
                    ),

                    plCreateVector2D(
                            _pl_console_pane[i].display.wh.x,
                            _pl_console_pane[i].display.wh.y
                    ),

                    plCreateColour4b(
                            (uint8_t) (_pl_console_pane[i].display.ul.r / 2),
                            (uint8_t) (_pl_console_pane[i].display.ul.g / 2),
                            (uint8_t) (_pl_console_pane[i].display.ul.b / 2),
                            _COLOUR_INACTIVE_ALPHA_TOP
                    ),

                    plCreateColour4b(
                            (uint8_t) (_pl_console_pane[i].display.ur.r / 2),
                            (uint8_t) (_pl_console_pane[i].display.ur.g / 2),
                            (uint8_t) (_pl_console_pane[i].display.ur.b / 2),
                            _COLOUR_INACTIVE_ALPHA_TOP
                    ),

                    plCreateColour4b(
                            (uint8_t) (_pl_console_pane[i].display.ll.r / 2),
                            (uint8_t) (_pl_console_pane[i].display.ll.g / 2),
                            (uint8_t) (_pl_console_pane[i].display.ll.b / 2),
                            _COLOUR_INACTIVE_ALPHA_BOTTOM
                    ),

                    plCreateColour4b(
                            (uint8_t) (_pl_console_pane[i].display.lr.r / 2),
                            (uint8_t) (_pl_console_pane[i].display.lr.g / 2),
                            (uint8_t) (_pl_console_pane[i].display.lr.b / 2),
                            _COLOUR_INACTIVE_ALPHA_BOTTOM
                    )
            ));

            plDrawRectangle(plCreateRectangle(

                    plCreateVector2D(
                            _pl_console_pane[i].display.xy.x + 4,
                            _pl_console_pane[i].display.xy.y + 4
                    ),

                    plCreateVector2D(
                            _pl_console_pane[i].display.wh.x - 8,
                            20
                    ),

                    plCreateColour4b(_COLOUR_HEADER_INACTIVE),
                    plCreateColour4b(_COLOUR_HEADER_INACTIVE),
                    plCreateColour4b(_COLOUR_HEADER_INACTIVE),
                    plCreateColour4b(_COLOUR_HEADER_INACTIVE)
            ));

            // todo, dim console title
        }

        // todo, display buffer size display
        // todo, display buffer text
    }

    if(!_pl_console_font) {
        return;
    }

#if defined(PL_MODE_OPENGL)
    glEnable(GL_TEXTURE_RECTANGLE);

    glBindTexture(GL_TEXTURE_RECTANGLE, _pl_console_font->texture->id);
#endif

    plDrawCharacter(_pl_console_font, 20, 20, 2, 'A');
    plDrawCharacter(_pl_console_font, 20, 30, 2, 'B');
    plDrawCharacter(_pl_console_font, 20, 40, 2, 'C');
    plDrawCharacter(_pl_console_font, 20, 50, 4, 'D');

#if defined(PL_MODE_OPENGL)
    glDisable(GL_TEXTURE_RECTANGLE);
#endif
}
