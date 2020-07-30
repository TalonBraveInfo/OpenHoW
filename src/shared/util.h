/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

#pragma once

#include <PL/platform.h>
#include <PL/platform_console.h>

enum LogLevel {
  LOG_LEVEL_DEFAULT,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_DEBUG,
};

#define u_unused( ... ) (void)( __VA_ARGS__ )

#define _print_w_function(LEVEL, FORMAT, ...) plLogMessage((LEVEL), "(%s) " FORMAT, PL_FUNCTION, ## __VA_ARGS__)

#ifdef _DEBUG
#   define LogDebug(...) _print_w_function(LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#   define LogDebug(...) ()
#endif

#define LogInfo(...)    _print_w_function(LOG_LEVEL_DEFAULT, __VA_ARGS__)
#define LogWarn(...)    _print_w_function(LOG_LEVEL_WARNING, __VA_ARGS__)
#if !defined(COMPILE_ENGINE)
#define Error(...) _print_w_function(LOG_LEVEL_ERROR, __VA_ARGS__); \
        u_assert(0, __VA_ARGS__);                                   \
        exit(EXIT_FAILURE)
#else
#define Error(...) _print_w_function(LOG_LEVEL_ERROR, __VA_ARGS__); \
        u_assert(0, __VA_ARGS__);                                   \
        System_DisplayMessageBox(PROMPT_LEVEL_ERROR, __VA_ARGS__);  \
        System_Shutdown()
#endif

typedef unsigned int uint;

#define u_fclose(FILE)  if((FILE) != NULL) { fclose((FILE)); (FILE) = NULL; }
#define u_free(DATA)    free((DATA)); (DATA) = NULL

#ifdef __cplusplus
static inline std::string u_stringtolower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}
#endif

PL_EXTERN_C

void u_init_logs(const char* log_path);

void* u_realloc(void* ptr, size_t new_size, bool abort_on_fail);
void* u_alloc(size_t num, size_t size, bool abort_on_fail);

const char* u_scan(const char* path, const char** preference);
const char* u_find2(const char* path, const char** preference, bool abort_on_fail);

FILE* u_open(const char* path, const char* mode, bool abort_on_fail);

char *u_new_filename( char *dst, const char *src, const char *ext );

PL_EXTERN_C_END

#ifdef _DEBUG
#   define u_assert(a, ...) if(!((a))) { LogWarn(__VA_ARGS__); LogInfo("Assertion hit in \"%s\" on line %d\n", PL_FUNCTION, __LINE__); } assert((a))
#else
#   define u_assert(a, ...) if(!((a))) { LogWarn(__VA_ARGS__); }
#endif
