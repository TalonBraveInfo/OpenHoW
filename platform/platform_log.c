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

#include <PL/platform_log.h>

/*	Log System	*/

#define LOG_FILE_EXTENSION  ".log"

void plWriteLog(const char *path, const char *msg, ...) {
    plFunctionStart();

    char newpath[PL_SYSTEM_MAX_PATH];
    sprintf(newpath, "%s"LOG_FILE_EXTENSION, path);

    static char buffer[1024];
    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, sizeof(buffer), msg, args);
    va_end(args);

    size_t size = strlen(buffer);
    FILE *file = fopen(newpath, "a");
    if(file != NULL) {
        if (fwrite(buffer, sizeof(char), size, file) != size) {
            _plSetErrorMessage("Failed to write to log! (%s)", newpath);
        }
        fclose(file);
    } else {
        // todo, needs to be more appropriate; return details on exact issue
        _plReportError(PL_RESULT_FILEREAD, "Failed to open %s!", newpath);
    }
}

void plClearLog(const char *path) {
    plFunctionStart();

    char newpath[PL_SYSTEM_MAX_PATH];
    sprintf(newpath, "%s"LOG_FILE_EXTENSION, path);
    unlink(newpath);
}

void plLog(unsigned int level, const char *msg, ...) {

}
