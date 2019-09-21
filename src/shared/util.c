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

#include "util.h"

#if defined(COMPILE_ENGINE)
# include "../engine/engine.h"
#endif

/****************************************************/
/* Logs */

void u_init_logs(const char* log_path) {
  plSetupLogOutput(log_path);

  /* set our logs up */

  plSetupLogLevel(LOG_LEVEL_DEFAULT, "info", PLColour(0, 255, 0, 255), true);
  plSetupLogLevel(LOG_LEVEL_WARNING, "warning", PLColour(255, 255, 0, 255), true);
  plSetupLogLevel(LOG_LEVEL_ERROR, "error", PLColour(255, 0, 0, 255), true);
  plSetupLogLevel(LOG_LEVEL_DEBUG, "debug", PLColour(0, 255, 255, 255),
#ifdef _DEBUG
                  true
#else
      false
#endif
  );
}

/****************************************************/
/* Memory */

unsigned long u_unmangle(void* source, void* destination) {
  /* todo */

  return 0;
}

void* u_realloc(void* ptr, size_t new_size, bool abort_on_fail) {
  void* mem = realloc(ptr, new_size);
  if(mem == NULL && new_size > 0) {
    if(abort_on_fail) {
      Error("Failed to allocate %u bytes!\n", new_size);
    }

    LogWarn("Failed to allocate %u bytes!\n", new_size);
    return NULL;
  }

  return mem;
}

void* u_alloc(size_t num, size_t size, bool abort_on_fail) {
  void* mem = calloc(num, size);
  if (mem == NULL && abort_on_fail) {
    Error("Failed to allocate %u bytes!\n", size * num);
  }
  return mem;
}

/****************************************************/
/* Filesystem */

PLConsoleVariable* cv_path_base = NULL;
PLConsoleVariable* cv_path_mod = NULL;

void u_init_paths(void) {
#define rvar(var, arc, ...) \
    { \
        const char *str_##var = plStringify(var); \
        (var) = plRegisterConsoleVariable(&str_##var[3], __VA_ARGS__); \
        (var)->archive = (arc); \
    }

  rvar(cv_path_base, true, ".", pl_string_var, NULL, "");
  rvar(cv_path_mod, false, "", pl_string_var, NULL, "");
}

const char* u_get_base_path(void) {
  u_assert(cv_path_base);
  return cv_path_base->s_value;
}
const char* u_get_mod_path(void) {
  u_assert(cv_path_mod);
  return cv_path_mod->s_value;
}
void u_set_base_path(const char* path) { plSetConsoleVariable(cv_path_base, path); }
void u_set_mod_path(const char* path) { plSetConsoleVariable(cv_path_mod, path); }

const char* u_get_full_path(void) {
  static char path[PL_SYSTEM_MAX_PATH];
  if(u_get_mod_path()[0] != '\0') {
    snprintf(path, sizeof(path), "%s/campaigns/%s", u_get_base_path(), u_get_mod_path());
  } else {
    snprintf(path, sizeof(path), "%s", u_get_base_path());
  }
  return path;
}

const char* u_scan(const char* path, const char** preference) {
  static char find[PL_SYSTEM_MAX_PATH];
  while (*preference != NULL) {
    snprintf(find, sizeof(find), "%s.%s", path, *preference);
    if (plFileExists(find)) {
      //LogDebug("Found \"%s\"\n", find);
      return find;
    }
    preference++;
  }

  //LogDebug("Failed to find \"%s\"\n", path);
  return "";
}

const char* u_find(const char* path) {
  static char n_path[PL_SYSTEM_MAX_PATH];
  if (!plIsEmptyString(u_get_mod_path())) {
    snprintf(n_path, sizeof(n_path), "%s/%s", u_get_full_path(), path);
    if (plFileExists(n_path)) {
      //LogDebug("Found \"%s\"\n", n_path);
      return n_path;
    }
  }

  snprintf(n_path, sizeof(n_path), "%s/%s", u_get_base_path(), path);
  //LogDebug("Found \"%s\"\n", n_path);
  return n_path;
}

const char* u_find2(const char* path, const char** preference, bool abort_on_fail) {
  static char out[PL_SYSTEM_MAX_PATH];
  memset(out, 0, sizeof(out));

  char base_path[PL_SYSTEM_MAX_PATH];
  if (!plIsEmptyString(u_get_mod_path())) {
    snprintf(base_path, sizeof(base_path), "%s/campaigns/%s/%s", u_get_base_path(), u_get_mod_path(), path);
    strncpy(out, u_scan(base_path, preference), sizeof(out));
    if (!plIsEmptyString(out)) {
      //LogDebug("Found \"%s\"\n", out);
      return out;
    }
  }

  snprintf(base_path, sizeof(base_path), "%s/%s", u_get_base_path(), path);
  strncpy(out, u_scan(base_path, preference), sizeof(out));
  if (plIsEmptyString(out)) {
    if (abort_on_fail) {
      Error("Failed to find \"%s\"!\n", path);
    }

    LogWarn("Failed to find \"%s\"!\n", path);
    return NULL;
  }

  //LogDebug("Found \"%s\"\n", out);
  return out;
}

FILE* u_open(const char* path, const char* mode, bool abort_on_fail) {
  FILE* fp = fopen(u_find(path), mode);
  if (fp == NULL) {
    /* todo: provide more detail! */
    if (abort_on_fail) {
      Error("Failed to open file at \"%s\"!\n", path);
    }

    LogWarn("Failed to open file at \"%s\"!\n", path);
    return NULL;
  }

  return fp;
}
