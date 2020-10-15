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

#define u_unused( ... ) (void)( __VA_ARGS__ )

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
#   define u_assert(a, ...) if(!((a))) { Warning(__VA_ARGS__); Print("Assertion hit in \"%s\" on line %d\n", PL_FUNCTION, __LINE__); } assert((a))
#else
#   define u_assert(a, ...) if(!((a))) { LogWarn(__VA_ARGS__); }
#endif
