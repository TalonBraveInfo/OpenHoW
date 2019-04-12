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
#include <PL/platform_filesystem.h>

#include "../input.h"
#include "../engine.h"
#include "../client/video.h"
#include "../game/TempGame.h"
#include "script.h"

#include "duktape-2.2.0/duktape.h"
#include "duktape-2.2.0/duk_module_duktape.h"

duk_context *scr_context = NULL;    /* server context */

#define CheckContext(A, B) if((A) != (B)) { LogWarn("invalid context, ignoring script function!\n"); return -1; }

static duk_ret_t SC_LoadScript(duk_context *context) {
    // todo!!!

    return 0;
}

static duk_ret_t SC_GetCampaignName(duk_context *context) {
#if 0
    ModManifest *cur = Mod_GetCurrentCampaign();
    if(cur == NULL) {
        duk_push_string(context, "null");
        return 0;
    }

    duk_push_string(context, cur->name);
#endif
    return 0;
}

static duk_ret_t SC_GetCurrentMapName(duk_context *context) {
    //duk_push_string(context, Map_GetCurrentName());
    return 0;
}

static duk_ret_t SC_GetCurrentMapDescription(duk_context *context) {
    //duk_push_string(context, Map_GetCurrentDescription());
    return 0;
}

static duk_ret_t SC_LogInfo(duk_context *context) {
    duk_push_string(context, " ");
    duk_insert(context, 0);
    duk_join(context, duk_get_top(context) - 1);
    LogInfo("%s", duk_safe_to_string(context, -1));
    return 0;
}

static duk_ret_t SC_LogWarn(duk_context *context) {
    duk_push_string(context, " ");
    duk_insert(context, 0);
    duk_join(context, duk_get_top(context) - 1);
    LogWarn("%s", duk_safe_to_string(context, -1));
    return 0;
}

static duk_ret_t SC_LogDebug(duk_context *context) {
    duk_push_string(context, " ");
    duk_insert(context, 0);
    duk_join(context, duk_get_top(context) - 1);
    LogDebug("%s", duk_safe_to_string(context, -1));
    return 0;
}

static duk_ret_t SC_Error(duk_context *context) {
    duk_push_string(context, " ");
    duk_insert(context, 0);
    duk_join(context, duk_get_top(context) - 1);
    Error("%s", duk_safe_to_string(context, -1));
}

/************************************************************/

#define pork_call(a, b) \
    if(duk_pcall((a), (b)) != 0) { \
        LogWarn("failed to call script function!\n %s\n", duk_safe_to_string((b), -1)); \
    }

void CS_InitGame(void) {
    duk_get_prop_string(scr_context, -1, "InitGame");
#if 0
    duk_push_int(scr_context, 10);
    duk_push_int(scr_context, 20);
#endif
    pork_call(scr_context, 0);
    duk_pop(scr_context);
}

/************************************************************/

void LoadScript(duk_context *context, const char *path) {
    LogInfo("loading \"%s\"...\n", path);

    size_t length = plGetFileSize(path);
    if(length == 0) {
        LogWarn("empty script, \"%s\"!\n", path);
        return;
    }

    FILE *fs = fopen(path, "rb");
    if(fs == NULL) {
        LogWarn("failed to load script, \"%s\"!\n", path);
        return;
    }

    char buf[length];
    if(fread(buf, sizeof(char), length, fs) != length) {
        LogWarn("failed to read complete script!\n");
    }
    fclose(fs);

    duk_push_lstring(context, buf, length);
    if(duk_peval(context) != 0) {
        LogWarn("failed to compile \"%s\"!\n %s\n", duk_safe_to_string(context, -1));
    } else {
        LogInfo("script compiled successfully\n");
    }
    duk_pop(context);

    duk_push_global_object(context);
}

static void DeclareGlobalString(duk_context *context, const char *name, const char *value) {
    duk_push_string(context, value);
    if(!duk_put_global_string(context, name)) {
        Error("failed to declare global string, \"%s\", aborting!\n", name);
    }
}

static void DeclareGlobalInteger(duk_context *context, const char *name, int value) {
    duk_push_int(context, value);
    if(!duk_put_global_string(context, name)) {
        Error("failed to declare global integer, \"%s\", aborting!\n", name);
    }
}

/* declare on both server and client */
#define DeclareUniversalGlobalString(value) \
    DeclareGlobalString(scr_context, #value, (value))
#define DeclareUniversalGlobalInteger(value) \
    DeclareGlobalInteger(scr_context, #value, (value))

typedef struct ScriptFunction {
    const char *name;
    int num_args;
    duk_ret_t (*Function)(duk_context *context);
} ScriptFunction;

#define _d(value, args) \
    {#value, (args), SC_##value},

ScriptFunction scr_builtins[]= {
        _d(LogInfo, DUK_VARARGS)
        _d(LogWarn, DUK_VARARGS)
        _d(LogDebug, DUK_VARARGS)
        _d(Error, DUK_VARARGS)

        _d(GetCampaignName, 0)
        _d(GetCurrentMapName, 0)
        _d(GetCurrentMapDescription, 0)
};

void Script_Initialize(void) {
    /* init the server context */

    scr_context = duk_create_heap_default();
    if(scr_context == NULL) {
        Error("failed to create heap for default context, aborting!\n");
    }

    for(unsigned int i = 0; i < plArrayElements(scr_builtins); ++i) {
        duk_push_c_function(scr_context, scr_builtins[i].Function, scr_builtins[i].num_args);
        duk_put_global_string(scr_context, scr_builtins[i].name);
    }

    duk_module_duktape_init(scr_context);

    /* pig classes */
    DeclareUniversalGlobalInteger(PIG_CLASS_ACE);
    DeclareUniversalGlobalInteger(PIG_CLASS_LEGEND);
    DeclareUniversalGlobalInteger(PIG_CLASS_MEDIC);
    DeclareUniversalGlobalInteger(PIG_CLASS_COMMANDO);
    DeclareUniversalGlobalInteger(PIG_CLASS_SPY);
    DeclareUniversalGlobalInteger(PIG_CLASS_SNIPER);
    DeclareUniversalGlobalInteger(PIG_CLASS_SABOTEUR);
    DeclareUniversalGlobalInteger(PIG_CLASS_HEAVY);
    DeclareUniversalGlobalInteger(PIG_CLASS_GRUNT);

    /* controller vars */
    DeclareUniversalGlobalInteger(PORK_BUTTON_CROSS);
    DeclareUniversalGlobalInteger(PORK_BUTTON_CIRCLE);
    DeclareUniversalGlobalInteger(PORK_BUTTON_TRIANGLE);
    DeclareUniversalGlobalInteger(PORK_BUTTON_SQUARE);
    DeclareUniversalGlobalInteger(PORK_BUTTON_R1);
    DeclareUniversalGlobalInteger(PORK_BUTTON_R2);
    DeclareUniversalGlobalInteger(PORK_BUTTON_R3);
    DeclareUniversalGlobalInteger(PORK_BUTTON_L1);
    DeclareUniversalGlobalInteger(PORK_BUTTON_L2);
    DeclareUniversalGlobalInteger(PORK_BUTTON_L3);
    DeclareUniversalGlobalInteger(PORK_BUTTON_UP);
    DeclareUniversalGlobalInteger(PORK_BUTTON_DOWN);
    DeclareUniversalGlobalInteger(PORK_BUTTON_LEFT);
    DeclareUniversalGlobalInteger(PORK_BUTTON_RIGHT);
    DeclareUniversalGlobalInteger(PORK_BUTTON_START);
    DeclareUniversalGlobalInteger(PORK_BUTTON_SELECT);

    DeclareUniversalGlobalInteger(PORK_MAX_BUTTONS);
    DeclareUniversalGlobalInteger(INPUT_MAX_CONTROLLERS);

    /* mouse vars */
    DeclareUniversalGlobalInteger(PORK_MOUSE_BUTTON_LEFT);
    DeclareUniversalGlobalInteger(PORK_MOUSE_BUTTON_RIGHT);
    DeclareUniversalGlobalInteger(PORK_MOUSE_BUTTON_MIDDLE);
    DeclareUniversalGlobalInteger(INPUT_MAX_MOUSE_BUTTONS);

    /* now init our scripts! */

    //LoadScript(scr_context, "./scripts/client.js");

    //duk_eval_string(scr_context, "LogInfo('Hello world!');");
    //printf("1+2=%d\n", (int)duk_get_int(scr_context, -1));
}

void Script_Shutdown(void) {
    if(scr_context != NULL) {
        duk_destroy_heap(scr_context);
    }
}
