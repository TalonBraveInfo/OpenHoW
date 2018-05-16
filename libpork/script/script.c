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
#include <PL/platform_filesystem.h>

#include "pork_engine.h"

#include "client/client_video.h"

#include "script.h"
#include "duk_module_duktape.h"

duk_context *scr_context = NULL;    /* server context */
duk_context *jsn_context = NULL;    /* json context */

#define CheckContext(A, B) if((A) != (B)) { LogWarn("invalid context, ignoring script function!\n"); return -1; }

duk_ret_t SC_LoadScript(duk_context *context) {
    // todo!!!

    return 0;
}

duk_ret_t SC_GetModName(duk_context *context) {
    duk_push_string(context, g_state.mod_name);
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

static duk_ret_t SC_QueueVideos(duk_context *context) {
    CheckContext(context, scr_context);

    //QueueVideos("", 0);
    return 0;
}

/*************************************************************/

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

/*************************************************************/

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

void DeclareGlobalString(duk_context *context, const char *name, const char *value) {
    duk_push_string(context, value);
    if(!duk_put_global_string(context, name)) {
        Error("failed to declare global string, \"%s\", aborting!\n", name);
    }
}

void DeclareGlobalInteger(duk_context *context, const char *name, int value) {
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

ScriptFunction scr_builtins[]= {
        {"LogInfo", DUK_VARARGS, SC_LogInfo},
        {"LogWarn", DUK_VARARGS, SC_LogWarn},
        {"LogDebug", DUK_VARARGS, SC_LogDebug},
        {"Error", DUK_VARARGS, SC_Error},

        {"GetModName", 0, SC_GetModName},
};

void InitScripting(void) {
    /* init the config context */

    jsn_context = duk_create_heap_default();
    if(jsn_context == NULL) {
        Error("failed to create heap for config context, aborting!\n");
    }

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

    /* ... globals ... */
    {
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
        DeclareUniversalGlobalInteger(PORK_MAX_CONTROLLERS);

        /* mouse vars */
        DeclareUniversalGlobalInteger(PORK_MOUSE_BUTTON_LEFT);
        DeclareUniversalGlobalInteger(PORK_MOUSE_BUTTON_RIGHT);
        DeclareUniversalGlobalInteger(PORK_MOUSE_BUTTON_MIDDLE);
        DeclareUniversalGlobalInteger(PORK_MAX_MOUSE_BUTTONS);
    }

    /* now init our scripts! */

    //LoadScript(scr_context, "./scripts/client.js");

    //duk_eval_string(scr_context, "LogInfo('Hello world!');");
    //printf("1+2=%d\n", (int)duk_get_int(scr_context, -1));
}

void ShutdownScripting(void) {
    if(scr_context != NULL) {
        duk_destroy_heap(scr_context);
    }
}

/*************************************************************/
/** JSON **/

#define LogMissingProperty(P)   LogWarn("failed to get JSON property \"%s\"!\n", (P))
#define LogInvalidArray(P)      LogWarn("invalid JSON array for property \"%s\"!\n", (P))

void ParseJSON(const char *buf) {
    duk_push_string(jsn_context, buf);
    duk_json_decode(jsn_context, -1);
}

void FlushJSON(void) {
    duk_pop(jsn_context);
}

unsigned int GetJSONIntIndexProperty(const char *property, unsigned int index) {
#if 0 // todo, revisit
    unsigned int v = 0;
    if(duk_get_prop_string(jsn_context, -1, property)) {
        if (duk_get_prop_index(jsn_context, -1, index)) {
            v = duk_get_uint(jsn_context, index);
        } else {
            LogWarn("failed to get JSON index \"%d\" in property \"%s\"!\n", index, property);
        }
    } else {
        LogWarn("failed to get JSON property \"%s\"!\n", property);
    }
    duk_pop(jsn_context);
    return v;
#else
    return 0;
#endif
}

unsigned int GetJSONArrayLength(const char *property) {
    if(!duk_get_prop_string(jsn_context, -1, property)) {
        duk_pop(jsn_context);

        LogMissingProperty(property);
        return 0;
    }

    if(!duk_is_array(jsn_context, -1)) {
        duk_pop(jsn_context);

        LogInvalidArray(property);
        return 0;
    }

    unsigned int len = (unsigned int)duk_get_length(jsn_context, -1);
    duk_pop(jsn_context);
    return len;
}

const char *GetJSONStringProperty(const char *property) {
    if(!duk_get_prop_string(jsn_context, -1, property)) {
        duk_pop(jsn_context);

        LogMissingProperty(property);
        return "null";
    }
    const char *str = duk_to_string(jsn_context, -1);
    duk_pop(jsn_context);
    return str;
}

int GetJSONIntProperty(const char *property) {
    if(!duk_get_prop_string(jsn_context, -1, property)) {
        duk_pop(jsn_context);

        LogMissingProperty(property);
        return 0;
    }
    int var = duk_to_int(jsn_context, -1);
    duk_pop(jsn_context);
    return var;
}
