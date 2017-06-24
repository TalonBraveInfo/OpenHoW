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

#pragma once

#include "platform.h"

PL_EXTERN_C

PL_EXTERN void plGetUserName(PLchar *out);

PL_EXTERN void plGetWorkingDirectory(PLchar *out);
PL_EXTERN void plSetWorkingDirectory(const char *path);

PL_EXTERN void plStripExtension(PLchar *dest, const PLchar *in);

PL_EXTERN const PLchar *plGetFileExtension(const PLchar *in);
PL_EXTERN const PLchar *plGetFileName(const PLchar *path);

PL_EXTERN void plScanDirectory(const PLchar *path, const PLchar *extension, void(*Function)(const char *filepath));

PL_EXTERN void plLowerCasePath(char *out);

PL_EXTERN bool plCreateDirectory(const char *path);

// File I/O ...

PL_EXTERN bool plFileExists(const char *path);
PL_EXTERN bool plPathExists(const char *path);

PL_EXTERN bool plCopyFile(const char *path, const char *dest);

PL_EXTERN bool plIsFileModified(time_t oldtime, const char *path);

PL_EXTERN time_t plGetFileModifiedTime(const PLchar *path);

PL_EXTERN int16_t plGetLittleShort(FILE *fin);
PL_EXTERN int32_t plGetLittleLong(FILE *fin);

PL_EXTERN_C_END
