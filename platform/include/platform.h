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

/*
Platform Library

This library includes standard platform headers,
gives you some standard functions to interact with
the system and includes defines for basic data-types that
you can use in your applications for easier multi-platform
support.
*/

// PL_IGNORE_SHARED_HEADERS
// PL_IGNORE_PLATFORM_HEADERS
// PL_IGNORE_STD_HEADERS
// PL_IGNORE_VIDEO_HEADERS

#define PL_INCLUDE_STD_BOOL

// Shared headers
#ifndef PL_IGNORE_SHARED_HEADERS

#	include <stdio.h>
#	include <stdlib.h>
#	include <stdarg.h>
#	include <stdlib.h>

#	ifdef PL_INCLUDE_STD_BOOL

#		include <stdbool.h>

#	endif
#	ifdef _MSC_VER    // MSVC doesn't support stdint...
#		ifndef __cplusplus
#			include "platform_inttypes.h"
#		endif
#	else

#		include <stdint.h>

#	endif

#	include <string.h>
#	include <ctype.h>
#	include <math.h>
#	include <setjmp.h>
#	include <errno.h>
#	include <time.h>

#	include <sys/stat.h>
#	include <sys/types.h>

// C++
#	ifndef PL_IGNORE_STD_HEADERS
#		ifdef __cplusplus
#			include <stdint.h>
#			include <memory>
#			include <string>
#			include <vector>
#			include <set>
#			include <unordered_set>
#			include <map>
#			include <unordered_map>
#			include <algorithm>

// istream
#			include <fstream>
#			include <iostream>
#		endif
#	endif
#endif

#ifdef __cplusplus
#	define	PL_EXTERN_C     extern "C" {
#	define	PL_EXTERN_C_END }
#else
#	define  PL_EXTERN_C
#	define  PL_EXTERN_C_END
#endif

#include "platform_system.h"

#if defined(_DEBUG)
#   include <assert.h>

#   define plAssert(a) assert(a)
#else
#   define plAssert(a) (a)
#endif

// These are usually expected to be defined already, but in-case they're not then we define them here.
#ifndef BOOL
#	define BOOL     bool
#endif
#ifndef TRUE
#	define TRUE     true
#endif
#ifndef FALSE
#	define FALSE    false
#endif
#define PL_BOOL     BOOL
#define PL_TRUE     TRUE
#define PL_FALSE    FALSE

typedef int                     PLint;
typedef long int                PLint32;
typedef unsigned int            PLuint;
typedef unsigned char           PLuint8;
typedef unsigned short int      PLuint16;
typedef unsigned long int       PLuint32;
typedef char                    PLchar;
typedef unsigned char           PLuchar, PLbyte;
#ifdef __cplusplus
typedef bool					PLbool;
#else
typedef unsigned char           PLbool;
#endif
typedef void                    PLvoid;
typedef float                   PLfloat;
typedef double                  PLdouble;
typedef short                   PLshort;
typedef unsigned short          PLushort;

#define plArrayElements(a)  (sizeof(a) / sizeof(*(a)))          // Returns the number of elements within an array.
#define plIsValidString(a)  ((a[0] != '\0') && (a[0] != ' '))

//////////////////////////////////////////////////////////////////

// Error return values
typedef enum {
    PL_RESULT_SUCCESS,

    // FILE I/O
    PL_RESULT_FILEREAD,     // Failed to read file!
    PL_RESULT_FILETYPE,     // Unexpected file type!
    PL_RESULT_FILEVERSION,  // Unsupported version!
    PL_RESULT_FILESIZE,     // Invalid file size!
    PL_RESULT_FILEPATH,     // Invalid path!

    // GRAPHICS
    PL_RESULT_GRAPHICSINIT,     // Graphics failed to initialise!
    PL_RESULT_SHADERTYPE,       // Unsupported shader type!
    PL_RESULT_SHADERCOMPILE,    // Failed to compile shader!

    // IMAGE
    PL_RESULT_IMAGERESOLUTION,  // Invalid image resolution!
    PL_RESULT_IMAGEFORMAT,      // Unsupported image format!

    // MEMORY
    PL_RESULT_MEMORYALLOC,    // Ran out of memory!
} PLresult;

//////////////////////////////////////////////////////////////////

enum {
    PL_SUBSYSTEM_GRAPHICS   = (1 << 0), // Graphics/rendering
    PL_SUBSYSTEM_IO         = (1 << 1), // Filesystem I/O
    PL_SUBSYSTEM_IMAGE      = (1 << 2), // Image loaders
    PL_SUBSYSTEM_LIBRARY    = (1 << 3), // Module/library management
    PL_SUBSYSTEM_LOG        = (1 << 4), // Logging
    PL_SUBSYSTEM_MODEL      = (1 << 5), // Model loaders
    PL_SUBSYSTEM_WINDOW     = (1 << 6), // Windowing
};

#if defined(PL_INTERNAL)

#define PL_DLL  PL_EXPORT

#ifndef __cplusplus
#define pFUNCTION_START  plSetErrorFunction(PL_FUNCTION); {
#else
#define	pFUNCTION_START	\
plSetErrorFunction(PL_FUNCTION);
// TRY whatever
#endif
#define pFUNCTION_END   }

// C Exceptions, Inspired by the following
// http://www.di.unipi.it/~nids/docs/longjump_try_trow_catch.html

#if 1

#define plFunctionStart()
#define plFunctionEnd()

#else

#include <setjmp.h>

#ifdef __cplusplus

    plFunctionStart() try { \
        plResetError(); plSetErrorFunction(PL_FUNCTION)
    plFunctionEnd() catch(...) { }

#else

#define plFunctionStart() do { jmp_buf ex_buf__; if(!setjmp(ex_buf__)) { \
    plResetError(); plSetErrorFunction(PL_FUNCTION)
#define plFunctionEnd() } else { } } } while(0)

#endif

#endif

PL_EXTERN_C

PLresult _plInitGraphics(void);
void _plShutdownGraphics(void);

PLresult _plInitIO(void);
void _plShutdownIO(void);

PLresult _plInitWindow(void);
void _plShutdownWindow(void);

PL_EXTERN_C_END

#else

#define PL_DLL  PL_IMPORT

#endif

PL_EXTERN_C

PL_EXTERN PLresult plInitialize(PLint argc, PLchar **argv, PLuint subsystems);
PL_EXTERN void plShutdown(void);

PL_EXTERN void plResetError(void); // Resets the error message to "null", so you can ensure you have the correct message from the library.
PL_EXTERN void
plSetError(const char *msg, ...);   // Sets the error message, so we can grab it outside the library.
PL_EXTERN void
plSetErrorFunction(const char *function, ...);  // Sets the currently active function, for error reporting.

PL_EXTERN const PLchar *plGetResultString(PLresult result);

PL_EXTERN const PLchar * plGetSystemError(void);  // Returns the error message currently given by the operating system.
PL_EXTERN const PLchar * plGetError(void);        // Returns the last recorded error.

// CL Arguments
PL_EXTERN const char *plGetCommandLineArgument(const char *arg);

PL_EXTERN_C_END

//////////////////////////////////////////////////////////////////

/*	Converts string to time.
	http://stackoverflow.com/questions/1765014/convert-string-from-date-into-a-time-t
*/
static PL_INLINE time_t plStringToTime(const PLchar *ts) {
    PLchar s_month[5];
    PLint day, year;
    sscanf(ts, "%s %d %d", s_month, &day, &year);

    static const PLchar months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    PLint month = (PLint)((strstr(months, s_month) - months) / 3);
    struct tm time = {0};
    time.tm_mon = month;
    time.tm_mday = day;
    time.tm_year = year - 1900;
    time.tm_isdst = -1;

    return mktime(&time);
}

static PL_INLINE PLbool plIsPowerOfTwo(PLuint num) {
    return (PLbool)((num != 0) && ((num & (~num + 1)) == num));
}

//////////////////////////////////////////////////////////////////
