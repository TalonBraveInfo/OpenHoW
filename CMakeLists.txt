#[[
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
]]

cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Set all of our output directories.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") #GCC
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsign-compare -Wno-unused-function -Wno-unused-variable -Wno-format-truncation -Wold-style-declaration")
endif()

add_definitions("-D_DEBUG")

function(build_openal)
    set(LIBTYPE "STATIC")
    add_definitions("-DAL_LIBTYPE_STATIC")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/3rdparty/openal-soft/lib/")
    set(ALSOFT_UTILS    OFF CACHE BOOL "Build and install utility programs")
    set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build and install example programs")
    set(ALSOFT_TESTS    OFF CACHE BOOL "Build and install test programs")
    add_subdirectory(src/3rdparty/openal-soft)
endfunction()

if(WIN32 OR APPLE)
    build_openal()
endif()

add_subdirectory(src/3rdparty/platform/platform)
add_subdirectory(src/tools/extractor)
add_subdirectory(src/engine)
