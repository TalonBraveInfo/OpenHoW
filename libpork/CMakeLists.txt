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

project(OpenHoW)

option(PORK_USE_IMGUI "ImGui" ON)

file(
        GLOB LIBPORK_SOURCE_FILES
        *.cpp *.c
        *.h

        script/duktape-2.2.0/*.c
        script/duktape-2.2.0/*.h
        script/*.cpp
        script/*.c
        script/*.h

        game/*.cpp game/*.c game/*.h

        server/*.cpp
        server/*.c
        server/*.h

        client/*.cpp
        client/*.c
        client/*.h

        include/pork/*.h
        )

if(PORK_USE_IMGUI)
        file(GLOB LIBPORK_IMGUI_SOURCE
                ../imgui/imgui.cpp
                ../imgui/imgui_draw.cpp
                ../imgui/imgui_widgets.cpp
                #../imgui/imgui_demo.cpp

                # todo: replace with internal implementation?
                ../imgui/examples/imgui_impl_opengl2.cpp
                ../imgui/examples/imgui_impl_opengl2.h
                )

        add_definitions("-DPORK_USE_IMGUI")
endif()

add_executable(OpenHoW ${LIBPORK_SOURCE_FILES} ${LIBPORK_IMGUI_SOURCE})

#set_target_properties(libpork PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/platform/lib/)

target_compile_options(OpenHoW PUBLIC -fPIC)
target_include_directories(OpenHoW PUBLIC ../imgui/ script/duktape-2.2.0/)
target_link_libraries(OpenHoW
        platform

        SDL2

        avformat
        avcodec
        avutil

        openal

        GLEW)
