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
#include "script.h"

#include "duktape.h"
#include "duk_module_duktape.h"

duk_context *cli_context = NULL;    /* client context */
duk_context *svr_context = NULL;    /* service context */
duk_context *jsn_context = NULL;    /* json context */

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
    return 0;
}

/*************************************************************/

#define pork_call(a, b) \
    if(duk_pcall((a), (b)) != 0) { \
        LogWarn("failed to call script function!\n %s\n", duk_safe_to_string((b), -1)); \
    }

/* Client */

void CS_InitClient(void) {
    duk_get_prop_string(cli_context, -1, "InitClientGame");
#if 0
    duk_push_int(scr_context, 10);
    duk_push_int(scr_context, 20);
#endif
    pork_call(cli_context, 0);
    duk_pop(cli_context);
}

/* Server */

void CS_InitServer(void) {
    duk_get_prop_string(svr_context, -1, "InitServerGame");
#if 0
    duk_push_int(scr_context, 10);
    duk_push_int(scr_context, 20);
#endif
    pork_call(svr_context, 0);
    duk_pop(svr_context);
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

typedef struct ScriptFunction {
    const char *name;
    int num_args;
    duk_ret_t (*Function)(duk_context *context);
} ScriptFunction;

ScriptFunction svr_builtins[]= {
        {"LogInfo", DUK_VARARGS, SC_LogInfo},
        {"LogWarn", DUK_VARARGS, SC_LogWarn},
        {"LogDebug", DUK_VARARGS, SC_LogDebug},
        {"Error", DUK_VARARGS, SC_Error},
};

ScriptFunction cli_builtins[]= {
        {"LogInfo", DUK_VARARGS, SC_LogInfo},
        {"LogWarn", DUK_VARARGS, SC_LogWarn},
        {"LogDebug", DUK_VARARGS, SC_LogDebug},
        {"Error", DUK_VARARGS, SC_Error},
};

void InitScripting(void) {
    /* init the config context */

    jsn_context = duk_create_heap_default();
    if(jsn_context == NULL) {
        Error("failed to create heap for config context, aborting!\n");
    }

    /* init the server context */

    svr_context = duk_create_heap_default();
    if(svr_context == NULL) {
        Error("failed to create heap for default context, aborting!\n");
    }

    for(unsigned int i = 0; i < plArrayElements(svr_builtins); ++i) {
        duk_push_c_function(svr_context, svr_builtins[i].Function, svr_builtins[i].num_args);
        duk_put_global_string(svr_context, svr_builtins[i].name);
    }

    duk_module_duktape_init(svr_context);

    /* init the client context */

    cli_context = duk_create_heap_default();
    if(cli_context == NULL) {
        Error("failed to create heap for default context, aborting!\n");
    }

    for(unsigned int i = 0; i < plArrayElements(cli_builtins); ++i) {
        duk_push_c_function(cli_context, cli_builtins[i].Function, cli_builtins[i].num_args);
        duk_put_global_string(cli_context, cli_builtins[i].name);
    }

    duk_module_duktape_init(cli_context);

    /* now init our scripts! */

    LoadScript(svr_context, "./scripts/server.js");
    LoadScript(cli_context, "./scripts/client.js");

    CS_InitServer();
    CS_InitClient();

    //duk_eval_string(scr_context, "LogInfo('Hello world!');");
    //printf("1+2=%d\n", (int)duk_get_int(scr_context, -1));
}

void ShutdownScripting(void) {
    if(svr_context != NULL) {
        duk_destroy_heap(svr_context);
    }
    if(cli_context != NULL) {
        duk_destroy_heap(cli_context);
    }
}

/*************************************************************/
/** JSON **/

void ParseJSON(const char *buf) {
    duk_push_string(jsn_context, buf);
    duk_json_decode(jsn_context, -1);
}

void FlushJSON(void) {
    duk_pop_2(jsn_context);
}

const char *GetJSONStringProperty(const char *property) {
    duk_get_prop_string(jsn_context, -1, property);
    return duk_to_string(jsn_context, -1);
}

int GetJSONIntProperty(const char *property) {
    duk_get_prop_string(jsn_context, -1, property);
    return duk_to_int(jsn_context, -1);
}
