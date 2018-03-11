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
#include "script.h"

#include "duktape.h"

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

    duk_eval_string(scr_context, "LogInfo('Hello world!');");

    //printf("1+2=%d\n", (int)duk_get_int(scr_context, -1));
}

void ShutdownScripting(void) {
    if(scr_context != NULL) {
        duk_destroy_heap(scr_context);
    }
}
