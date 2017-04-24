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

typedef struct PLModuleFunction {
    const PLchar *name;

    void **Function;
} PLModuleFunction;

PL_EXTERN_C

PL_EXTERN PL_FARPROC plFindLibraryFunction(PL_INSTANCE instance, const PLchar *function);

PL_EXTERN PLvoid *plLoadLibraryInterface(PL_INSTANCE instance, const PLchar *path, const PLchar *entry, PLvoid *handle);

PL_INSTANCE plLoadLibrary(const PLchar *path);    // Loads new library instance.
PL_EXTERN PLvoid plUnloadLibrary(PL_INSTANCE instance);    // Unloads library instance.

PL_EXTERN_C_END
