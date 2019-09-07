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

#include "../engine.h"
#include "script.h"
#include "duktape-2.2.0/duktape.h"

static duk_context* global_ctx = nullptr;

static duk_ret_t SFunc_doFile(duk_context* ctx) {
  const char* path = duk_require_string(ctx, 0);
  if (!Script_LoadFile(path)) {
    return duk_generic_error(ctx, "failed to load script: %s", path);
  }
  duk_call(ctx, 0);
  return 1;
}

static duk_ret_t SFunc_getCampaignName(duk_context* context) {
#if 0
  ModManifest *cur = Mod_GetCurrentCampaign();
  if(cur == NULL) {
      duk_push_string(context, "null");
      return 0;
  }

  duk_push_string(context, cur->name);
#endif
  return 1;
}

static duk_ret_t SFunc_getCurrentMapName(duk_context* context) {
  //duk_push_string(context, Map_GetCurrentName());
  return 1;
}

static duk_ret_t SFunc_getCurrentMapDescription(duk_context* context) {
  //duk_push_string(context, Map_GetCurrentDescription());
  return 1;
}

static duk_ret_t SFunc_logInfo(duk_context* context) {
  duk_push_string(context, " ");
  duk_insert(context, 0);
  duk_join(context, duk_get_top(context) - 1);
  LogInfo("%s", duk_safe_to_string(context, -1));
  return 1;
}

static duk_ret_t SFunc_logWarn(duk_context* context) {
  duk_push_string(context, " ");
  duk_insert(context, 0);
  duk_join(context, duk_get_top(context) - 1);
  LogWarn("%s", duk_safe_to_string(context, -1));
  return 1;
}

static duk_ret_t SFunc_logDebug(duk_context* context) {
  duk_push_string(context, " ");
  duk_insert(context, 0);
  duk_join(context, duk_get_top(context) - 1);
  LogDebug("%s", duk_safe_to_string(context, -1));
  return 1;
}

static duk_ret_t SFunc_error(duk_context* context) {
  duk_push_string(context, " ");
  duk_insert(context, 0);
  duk_join(context, duk_get_top(context) - 1);
  Error("%s", duk_safe_to_string(context, -1));
}

typedef struct ScriptFunction {
  const char* name;
  int num_args;
  duk_ret_t (* Function)(duk_context* context);
} ScriptFunction;

#define _d(value, args) \
    {#value, (args), SFunc_##value},

ScriptFunction scr_builtins[] = {
    _d(doFile, 1)

    _d(logInfo, DUK_VARARGS)
    _d(logWarn, DUK_VARARGS)
    _d(logDebug, DUK_VARARGS)
    _d(error, DUK_VARARGS)

    _d(getCampaignName, 0)
    _d(getCurrentMapName, 0)
    _d(getCurrentMapDescription, 0)
};

/************************************************************/

#define _call(a, b) \
    if(duk_pcall((a), (b)) != 0) { \
        LogWarn("Failed to call script function (%s)!\n", duk_safe_to_string((b), -1)); \
    }

void CF_initGame() {
  if (!duk_get_prop_string(global_ctx, -1, "initGame")) {
    duk_pop(global_ctx);
    return;
  }

#if 0
    duk_push_int(scr_context, 10);
    duk_push_int(scr_context, 20);
#endif

  _call(global_ctx, 0);
  duk_pop(global_ctx);
}

void CF_gameTick() {
  if (!duk_get_prop_string(global_ctx, -1, "gameTick")) {
    duk_pop(global_ctx);
    return;
  }

  _call(global_ctx, 0);
  duk_pop(global_ctx);
}

/************************************************************/

bool Script_LoadFile(const char* path) {
  LogInfo("Loading \"%s\"...\n", path);

  size_t length = plGetFileSize(path);
  if (length == 0) {
    LogWarn("Empty script, \"%s\"!\n", path);
    return false;
  }

  FILE* fs = fopen(path, "rb");
  if (fs == nullptr) {
    LogWarn("Failed to load script, \"%s\"!\n", path);
    return false;
  }

  char buf[length];
  if (fread(buf, sizeof(char), length, fs) != length) {
    LogWarn("Failed to read complete script!\n");
  }
  fclose(fs);

  if (buf[0] == '\0') {
    return false;
  }

  bool status = false;
  duk_push_lstring(global_ctx, buf, length);
  if (duk_peval(global_ctx) != 0) {
    LogWarn("Failed to compile \"%s\"!\n %s\n", duk_safe_to_string(global_ctx, -1));
  } else {
    LogInfo("Script compiled successfully\n");
    status = true;
  }
  duk_pop(global_ctx);

  duk_push_global_object(global_ctx);

  return status;
}

void Script_DeclareGlobalString(const char* name, const char* value) {
  duk_push_string(global_ctx, value);
  if (!duk_put_global_string(global_ctx, name)) {
    Error("Failed to declare global string, \"%s\", aborting!\n", name);
  }
}

void Script_DeclareGlobalInteger(const char* name, int value) {
  duk_push_int(global_ctx, value);
  if (!duk_put_global_string(global_ctx, name)) {
    Error("Failed to declare global integer, \"%s\", aborting!\n", name);
  }
}

static void FatalError(void* udata, const char* msg) {
  Error("Encountered script fault!\n%s\n", (msg ? msg : "NONE"));
}

void* s_alloc(void* data, duk_size_t size) {
  u_unused(data);
  return u_alloc(1, size, true);
}

void* s_realloc(void* data, void* ptr, duk_size_t size) {
  u_unused(data);
  return u_realloc(ptr, size, true);
}

void s_free(void* data, void* ptr) {
  u_unused(data);
  u_free(ptr);
}

void Script_Initialize(void) {
  global_ctx = duk_create_heap(
      s_alloc,
      s_realloc,
      s_free,
      nullptr,
      FatalError
  );
  if (global_ctx == nullptr) {
    Error("Failed to create heap for default context, aborting!\n");
  }

  for (auto& scr_builtin : scr_builtins) {
    duk_push_c_function(global_ctx, scr_builtin.Function, scr_builtin.num_args);
    duk_put_global_string(global_ctx, scr_builtin.name);
  }

  /* now init our scripts! */

  Script_LoadFile("./scripts/init.js");
}

void Script_EvaluateString(const char* str) {
  if (str == nullptr) {
    return;
  }

  duk_eval_string(global_ctx, str);
}

void Script_Shutdown(void) {
  if (global_ctx != nullptr) {
    duk_destroy_heap(global_ctx);
  }
}
