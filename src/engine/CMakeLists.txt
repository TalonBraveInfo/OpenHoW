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

#Required for OpenGL3.x imgui setup
add_definitions("-DIMGUI_IMPL_OPENGL_LOADER_GLEW")

file(
        GLOB OPENHOW_SOURCE_FILES
        *.cpp *.c
        *.h

        script/duktape-2.2.0/*.c
        script/duktape-2.2.0/*.h

        audio/audio.cpp
        graphics/*.*
        script/*.*
        game/*.*
        loaders/*.*

        ################## ImGui

        ../3rdparty/imgui/imgui.cpp
        ../3rdparty/imgui/imgui_draw.cpp
        ../3rdparty/imgui/imgui_widgets.cpp
        #../3rdparty/imgui/imgui_demo.cpp
        ../3rdparty/imgui/examples/imgui_impl_opengl3.cpp
        ../3rdparty/imgui/examples/imgui_impl_opengl3.h
        ../3rdparty/imgui_tabs/imgui_tabs.cpp
        )

add_executable(OpenHoW ${OPENHOW_SOURCE_FILES})

if(WIN32)
    add_dependencies(OpenHoW OpenAL)

    target_include_directories(OpenHoW PRIVATE
            ../3rdparty/SDL2/include/
            ../3rdparty/openal-soft/include/
            # below is hack to get imgui compiled on Windows with GLEW.
            ../3rdparty/platform/platform/3rdparty/glew-2.2.0/include/
            )
    target_link_directories(OpenHoW PRIVATE ../3rdparty/SDL2/lib/ ../3rdparty/openal-soft/lib/ ../../lib/)
    target_link_options(OpenHoW PRIVATE -mwindows)
    target_link_libraries(OpenHoW -Wl,-Bstatic SDL2 OpenAL32 stdc++ winpthread -Wl,-Bdynamic -static-libstdc++ -static-libgcc
            # Window Libraries
            Version SetupAPI Winmm Imm32)
elseif(APPLE)
    add_dependencies(OpenHoW OpenAL)

    target_include_directories(OpenHoW PRIVATE ../3rdparty/openal-soft/include/)
    target_link_libraries(OpenHoW OpenAL SDL2)
else()
    target_link_libraries(OpenHoW openal SDL2)
endif()

#set_target_properties(src PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/platform/lib/)

target_compile_options(OpenHoW PUBLIC -fPIC)
target_include_directories(OpenHoW PUBLIC ../3rdparty/imgui/ script/duktape-2.2.0/)
target_link_libraries(OpenHoW
        platform

        #avformat
        #avcodec
        #avutil
        )
