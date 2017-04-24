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

#include <platform_filesystem.h>
#include "platform.h"

/*	Generic functions for platform, such as	error handling.	*/

typedef struct PLSubSystem {
    PLuint subsystem;

    PLresult(*InitFunction)(void);
    PLvoid(*ShutdownFunction)(void);

    PLbool active;
} PLSubSystem;

PLSubSystem pl_subsystems[]= {
        {
                PL_SUBSYSTEM_GRAPHICS,
                &_plInitGraphics,
                &_plShutdownGraphics,
                PL_FALSE
        },

        {
                PL_SUBSYSTEM_IO,
                &_plInitIO,
                &_plShutdownIO,
                PL_FALSE
        },

        {
                PL_SUBSYSTEM_IMAGE,
                NULL,
                NULL,
                PL_FALSE
        },

        {
                PL_SUBSYSTEM_LIBRARY,
                NULL,
                NULL,
                PL_FALSE
        },

        {
                PL_SUBSYSTEM_WINDOW,
                NULL,
                NULL,
                PL_FALSE
        },

        {
                PL_SUBSYSTEM_MODEL,
                NULL,
                NULL,
                PL_FALSE
        }
};

typedef struct PLArgument {
    const char *parm;

    void*(*Callback)();
} PLArgument;

typedef struct PLArguments {
    const char *exe_name;
    const char *arguments[256];

    unsigned int num_arguments;
} PLArguments;

PLArguments pl_arguments;

PLresult plInitialize(PLint argc, PLchar **argv, PLuint subsystems) {
    for(unsigned int i = 0; i < plArrayElements(pl_subsystems); i++) {
        if(!pl_subsystems[i].active && (subsystems & pl_subsystems[i].subsystem)) {
            if(pl_subsystems[i].InitFunction) {
                PLresult out = pl_subsystems[i].InitFunction();
                if (out != PL_RESULT_SUCCESS) {
                    return out;
                }
            }

            pl_subsystems[i].active = PL_TRUE;
        }
    }

    memset(&pl_arguments, 0, sizeof(PLArguments));
    pl_arguments.num_arguments = (unsigned int)argc;
    if(plIsValidString(argv[0])) {
        pl_arguments.exe_name = plGetFileName(argv[0]);
    }

    //pl_arguments.arguments = (const char**)calloc(pl_arguments.num_arguments, sizeof(char*));
    for(unsigned int i = 0; i < pl_arguments.num_arguments; i++) {
        if(!plIsValidString(argv[i])) {
            continue;
        }

        pl_arguments.arguments[i] = argv[i];
    }

    return PL_RESULT_SUCCESS;
}

// Returns the name of the current executable.
const char *plGetExecutableName(void) {
    return pl_arguments.exe_name;
}

const char *plGetCommandLineArgument(const char *arg) {
    if(pl_arguments.num_arguments < 2) {
        return '\0';
    } else if(!plIsValidString(arg)) {
        // todo, get current log output and print warning there?
        return '\0';
    }

    for(unsigned int i = 0; i < pl_arguments.num_arguments; i++) {
        if(!strcmp(pl_arguments.arguments[i], arg)) {
            return pl_arguments.arguments[i + 1];
        }
    }

    return '\0';
}

void plShutdown(void) {
    for(PLuint i = 0; i < plArrayElements(pl_subsystems); i++) {
        if(!pl_subsystems[i].active) {
            continue;
        }

        if(pl_subsystems[i].ShutdownFunction) {
            pl_subsystems[i].ShutdownFunction();
        }

        pl_subsystems[i].active = PL_FALSE;
    }
}

/*	ERROR HANDLING	*/

#define    MAX_FUNCTION_LENGTH    64
#define    MAX_ERROR_LENGTH    2048

PLchar
        sys_error[MAX_ERROR_LENGTH]         = { '\0' },
        loc_error[MAX_ERROR_LENGTH]         = { '\0' },
        loc_function[MAX_FUNCTION_LENGTH]   = { '\0' };

// Sets the name of the current function.
void plSetErrorFunction(const char *function, ...) {
#ifdef _DEBUG
    PLchar out[2048]; // todo, shitty work around because linux crap    //[MAX_FUNCTION_LENGTH];
    va_list args;

    va_start(args, function);
    vsprintf(out, function, args);
    va_end(args);

    strncpy(loc_function, out, sizeof(function));
#endif
}

void plResetError(void) {
#ifdef _DEBUG
    loc_function[0] = loc_error[0] = sys_error[0] = '\0';
#endif
}

// Sets the local error message.
void plSetError(const char *msg, ...) {
#ifdef _DEBUG
    PLchar out[MAX_ERROR_LENGTH];
    va_list args;

    va_start(args, msg);
    vsprintf(out, msg, args);
    va_end(args);

    strncpy(loc_error, out, sizeof(loc_error));
#endif
}

// Returns locally generated error message.
const PLchar * plGetError(void) {
    return loc_error;
}

/*	Returns a system error message.
*/
const PLchar * plGetSystemError(void) {
#ifdef _WIN32
    char	*buffer = NULL;
    int		error;

    error = GetLastError();
    if (error == 0)
        return "Unknown system error!";

    if (!FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&buffer,
        0, NULL))
        return "Failed to get system error details!";

    strcpy(sys_error, _strdup(buffer));

    LocalFree(buffer);

    return sys_error;
#else
    strcpy(sys_error, dlerror());
    return sys_error;
#endif
}

const PLchar *plGetResultString(PLresult result) {
    switch (result) {
        case PL_RESULT_SUCCESS: return "Success";

        // FILE I/O
        case PL_RESULT_FILEREAD:    return "Failed to read complete file!";
        case PL_RESULT_FILESIZE:    return "Failed to get valid file size!";
        case PL_RESULT_FILETYPE:    return "Invalid file type!";
        case PL_RESULT_FILEVERSION: return "Unsupported file version!";
        case PL_RESULT_FILEPATH:    return "Invalid file path!";

        // GRAPHICS
        case PL_RESULT_GRAPHICSINIT:    return "Failed to initialize graphics!";
        case PL_RESULT_SHADERTYPE:      return "Unsupported shader type!";
        case PL_RESULT_SHADERCOMPILE:   return "Failed to compile shader!";

        // IMAGE
        case PL_RESULT_IMAGERESOLUTION: return "Invalid image resolution!";
        case PL_RESULT_IMAGEFORMAT:     return "Unsupported image format!";

        // MEMORY
        case PL_RESULT_MEMORYALLOC: return "Failed to allocate memory!";

        default:    return "An unknown error occurred!";
    }
}
