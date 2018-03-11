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

duk_context *scr_context = NULL;

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

#define pork_call(b) \
    if(duk_pcall(scr_context, (b)) != 0) { \
        LogWarn("failed to call script function!\n %s\n", duk_safe_to_string((b), -1)); \
    }

void CS_InitGame(void) {
    duk_get_prop_string(scr_context, -1, "InitGame");
#if 0
    duk_push_int(scr_context, 10);
    duk_push_int(scr_context, 20);
#endif
    pork_call(0);
    duk_pop(scr_context);
}

/*************************************************************/

void LoadScript(const char *path) {
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

    duk_push_lstring(scr_context, buf, length);
    if(duk_peval(scr_context) != 0) {
        LogWarn("failed to compile \"%s\"!\n %s\n", duk_safe_to_string(scr_context, -1));
    } else {
        LogInfo("script compiled successfully\n");
    }
    duk_pop(scr_context);

    duk_push_global_object(scr_context);
}

void InitScripting(void) {
    scr_context = duk_create_heap_default();
    if(scr_context == NULL) {
        Error("failed to create heap for default context, aborting!\n");
    }

    duk_push_c_function(scr_context, SC_LogInfo, DUK_VARARGS);
    duk_put_global_string(scr_context, "LogInfo");
    duk_push_c_function(scr_context, SC_LogWarn, DUK_VARARGS);
    duk_put_global_string(scr_context, "LogWarn");
    duk_push_c_function(scr_context, SC_LogDebug, DUK_VARARGS);
    duk_put_global_string(scr_context, "LogDebug");
    duk_push_c_function(scr_context, SC_Error, DUK_VARARGS);
    duk_put_global_string(scr_context, "Error");

    duk_module_duktape_init(scr_context);

    LoadScript("./scripts/init.js");

    /* now call up the function */
    CS_InitGame();
    CS_InitGame();
    CS_InitGame();

    //duk_eval_string(scr_context, "LogInfo('Hello world!');");
    //printf("1+2=%d\n", (int)duk_get_int(scr_context, -1));
}

void ShutdownScripting(void) {
    if(scr_context != NULL) {
        duk_destroy_heap(scr_context);
    }
}
