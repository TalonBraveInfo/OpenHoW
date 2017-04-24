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

/* This document provides platform-specific
 * headers and any additional system information.
 */

// Operating System

#if defined(_WIN32)

#	ifndef PL_IGNORE_PLATFORM_HEADERS
#		include <Windows.h>
#		include <WindowsX.h>
#		include <CommCtrl.h>
#		include <direct.h>
#		include <lmcons.h>

#		ifdef PlaySound
#			undef PlaySound
#		endif
#		ifdef LoadImage
#			undef LoadImage
#		endif

#		undef min
#		undef max
#	endif

#   define PL_SYSTEM_NAME               "WINDOWS"
#   define PL_SYSTEM_LIBRARY_EXTENSION  ".dll"

#   define PL_SYSTEM_MAX_PATH           (MAX_PATH - 1)
#   define PL_SYSTEM_MAX_USERNAME       UNLEN

#	ifdef _MSC_VER
#		pragma warning(disable : 4152)
#		pragma warning(disable : 4800)	// 'type' : forcing value to bool 'true' or 'false' (performance warning)

#		ifndef itoa
#			define	itoa		_itoa
#		endif
#		ifndef getcwd
#			define	getcwd		_getcwd
#		endif
#		ifndef snprintf
#			define	snprintf	_snprintf
#		endif
#		ifndef unlink
#			define	unlink		_unlink
#		endif
#		ifndef strcasecmp
#			define	strcasecmp	_stricmp
#		endif
#		ifndef mkdir
#			define	mkdir		_mkdir
#		endif
#		ifndef strncasecmp
#			define	strncasecmp	_str
#		endif
#		ifdef __cplusplus
#			ifndef nothrow
//#				define nothrow __nothrow
#			endif
#		endif
#	endif

#elif defined(__linux__) // Linux

#	ifndef PL_IGNORE_SYSTEM_HEADERS

#		include <dirent.h>
#		include <unistd.h>
#		include <dlfcn.h>
#		include <strings.h>

#	endif

#   define PL_SYSTEM_NAME   "LINUX"
#   define PL_SYSTEM_LIBRARY_EXTENSION ".so"

#   define PL_SYSTEM_MAX_PATH       256
#   define PL_SYSTEM_MAX_USERNAME   32

#elif defined(__APPLE__)

#   ifndef PL_IGNORE_SYSTEM_HEADERS
#       include <zconf.h>
#       include <dirent.h>
#       include <dlfcn.h>
#   endif

#   define PL_SYSTEM_NAME               "macOS"
#   define PL_SYSTEM_LIBRARY_EXTENSION  ".so"

#   define PL_SYSTEM_MAX_PATH       512
#   define PL_SYSTEM_MAX_USERNAME   32

#else

#   error "Unsupported system type."

#endif

// (Basic) Hardware

#if defined(__amd64) || defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)

#   define PL_SYSTEM_CPU    "AMD64"

#elif defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT)

#   define PL_SYSTEM_CPU    "ARM"

#elif defined(__aarch64__)

#   define PL_SYSTEM_CPU    "ARM64"

#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_I86) || defined(_M_IX86) || defined(_X86_)

#   define PL_SYSTEM_CPU    "INTEL86"

#else

#   error "Unsupported CPU type."

#endif

// Compiler

#if defined(_MSC_VER)
#   define PL_INSTANCE  HINSTANCE
#   define PL_FARPROC   FARPROC
#	define PL_EXTERN	extern
#	define PL_CALL		__stdcall
#	define PL_INLINE	__inline

// MSVC doesn't support __func__
#	define PL_FUNCTION	__FUNCTION__    // Returns the active function.

#   define PL_EXPORT    __declspec(dllexport)
#   define PL_IMPORT    __declspec(dllimport)
#else
#   define PL_INSTANCE  void*
#   define PL_FARPROC   void*
#	define PL_EXTERN    extern
#	define PL_CALL
#	define PL_INLINE    inline

#if 0
#	define PL_FUNCTION  __FILE__      // Returns the active function.
#else
#   define PL_FUNCTION __FUNCTION__
// todo, we'll need to do some weird hacky shit on Linux for this, since __func__ isn't a string literal like it is
#endif
// on MSVC

#   define PL_EXPORT    __attribute__((visibility("default")))
#   define PL_IMPORT    __attribute__((visibility("hidden")))
#endif
