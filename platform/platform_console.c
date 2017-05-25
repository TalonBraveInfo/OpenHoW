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

#include <PL/platform_console.h>
#include <PL/platform_graphics.h>

/* Multi Console Manager */
// todo, should the console be case-sensitive?
// todo, mouse input callback
// todo, keyboard input callback

typedef struct PLConsoleCommand {
    char cmd[24];

    void(*Callback)(unsigned int argc, char *argv[]);

    char description[512];
} PLConsoleCommand;

#define IMPLEMENT_COMMAND(a) void a(unsigned int argc, char *argv[])

IMPLEMENT_COMMAND(CLSCommand) {}
IMPLEMENT_COMMAND(COLOURCommand) {}
IMPLEMENT_COMMAND(TIMECommand) {}
IMPLEMENT_COMMAND(MEMCommand) {}
IMPLEMENT_COMMAND(HELPCommand) {}

PLConsoleCommand **_pl_commands = NULL;
PLConsoleCommand _pl_base_commands[]={
        { "cls", CLSCommand, "Clears the console buffer." },
        { "colour", COLOURCommand, "Changes the colour of the current console." },
        { "time", TIMECommand },
        { "mem", MEMCommand },
        { "help", HELPCommand },
};

size_t _pl_num_commands = 0;
size_t _pl_commands_size = 512;

void plRegisterConsoleCommands(PLConsoleCommand cmds[], unsigned int num_cmds) {
    if(num_cmds > _pl_commands_size) {

    }

    for(unsigned int i = 0; i < num_cmds; i++) {
        if((cmds[i].cmd[0] == '\0') || !(cmds[i].Callback)) {
            continue;
        }

        // todo, necessary to scan through? We know where the last slot was...
        PLConsoleCommand *cmd = _pl_commands[0];
        for (size_t j = 0; j < _pl_commands_size; j++, cmd++) {
            if (!cmd) {
                cmd = (PLConsoleCommand *) malloc(sizeof(PLConsoleCommand));
                if (!cmd) {
                    plSetError("malloc(%d)\n", sizeof(PLConsoleCommand));
                    break;
                }
                memcpy(cmd, &cmds[i], sizeof(PLConsoleCommand));
                break;
            }
        }
    }
}

void plRegisterConsoleCommand(PLConsoleCommand cmd) {

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

PLMesh *_pl_mesh_line = NULL;

PLresult _plInitConsole(void) {
    plFunctionStart();

    _pl_console_visible = false;

    memset(&_pl_console_pane, 0, sizeof(PLConsolePane) * PLCONSOLE_MAX_INSTANCES);
    _pl_active_console_pane = _pl_num_console_panes = 0;

    _pl_mesh_line = plCreateMesh(PL_PRIMITIVE_LINES, PL_DRAW_IMMEDIATE, 0, 4);
    if(!_pl_mesh_line) {
        return PL_RESULT_MEMORYALLOC;
    }

    _pl_commands = (PLConsoleCommand**)malloc(sizeof(PLConsoleCommand) * _pl_commands_size);
    if(!_pl_commands) {
        plSetError("malloc(%d * %d)\n", sizeof(PLConsoleCommand), _pl_commands_size);
        return PL_RESULT_MEMORYALLOC;
    }

    plRegisterConsoleCommands(_pl_base_commands, plArrayElements(_pl_base_commands));

    return PL_RESULT_SUCCESS;
}

void _plShutdownConsole(void) {
    plFunctionStart();

    _pl_console_visible = false;

    memset(&_pl_console_pane, 0, sizeof(PLConsolePane) * PLCONSOLE_MAX_INSTANCES);
    _pl_active_console_pane = _pl_num_console_panes = 0;

    if(_pl_commands) {
        PLConsoleCommand *cmd = _pl_commands[0];
        for (size_t i = 0; i < _pl_num_commands; i++, cmd++) {
            // todo, should we return here; assume it's the end?
            if (!cmd) {
                continue;
            }

            free(cmd);
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

        _pl_console_pane[i].display.width = width;
        _pl_console_pane[i].display.height = height;
        _pl_console_pane[i].display.x = position_x;
        _pl_console_pane[i].display.y = position_y;
    }
}

bool _plConsolePaneVisible(unsigned int id) {
    if(!_pl_console_visible) {
        return false;
    } else if(
        _pl_console_pane[id].display.ll.a == 0 &&
        _pl_console_pane[id].display.lr.a == 0 &&
        _pl_console_pane[id].display.ul.a == 0 &&
        _pl_console_pane[id].display.ur.a == 0 ) {
        return false;
    }

    return true;
}

// INPUT

void _plConsoleInput(int m_x, int m_y) {

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
        if(!_plConsolePaneVisible(i)) {
            continue;
        }

        if(i == _pl_active_console_pane) {
            plDrawRectangle(_pl_console_pane[i].display);

            plDrawRectangle(plCreateRectangle(
                    _pl_console_pane[i].display.x + 4,
                    _pl_console_pane[i].display.y + 4,
                    _pl_console_pane[i].display.width - 8,
                    20,

                    plCreateColour4b(_COLOUR_HEADER_ACTIVE_TOP),
                    plCreateColour4b(_COLOUR_HEADER_ACTIVE_TOP),
                    plCreateColour4b(_COLOUR_HEADER_ACTIVE_BOTTOM),
                    plCreateColour4b(_COLOUR_HEADER_ACTIVE_BOTTOM)
            ));

            // todo, console title
            // todo, display scroll bar
        } else {
            plDrawRectangle(plCreateRectangle(
                    _pl_console_pane[i].display.x,
                    _pl_console_pane[i].display.y,
                    _pl_console_pane[i].display.width,
                    _pl_console_pane[i].display.height,

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
                    _pl_console_pane[i].display.x + 4,
                    _pl_console_pane[i].display.y + 4,
                    _pl_console_pane[i].display.width - 8,
                    20,

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
}
