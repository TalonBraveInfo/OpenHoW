/* OpenHOW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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
#include <pork/pork.h>

#include <SDL2/SDL.h>

SDL_Window *window = NULL;

#define print(...)  plLogMessage(PORK_LOG_LAUNCHER, __VA_ARGS__)

void IDisplayMessageBox(unsigned int level, const char *msg, ...) {
    switch(level) {
        case PORK_MBOX_ERROR: {
            level = SDL_MESSAGEBOX_ERROR;
        } break;
        case PORK_MBOX_WARNING: {
            level = SDL_MESSAGEBOX_WARNING;
        } break;
        case PORK_MBOX_INFORMATION: {
            level = SDL_MESSAGEBOX_INFORMATION;
        } break;

        default: return;
    }

    char buf[2048] = {'\0'};
    va_list args;
    va_start(args, msg);
    vsnprintf(buf, sizeof(buf), msg, args);
    va_end(args);

    SDL_ShowSimpleMessageBox(level, PORK_TITLE, msg, window);
}

void IDisplayViewport(unsigned int width, unsigned int height) {
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

void IShutdownLauncher(void) {
    SDL_Quit();

    exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////

int main(int argc, char **argv) {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        IDisplayMessageBox(PORK_MBOX_ERROR, "Failed to initialize SDL2!\n%s", SDL_GetError());
        return EXIT_FAILURE;
    }

    PorkLauncherInterface interface;
    memset(&interface, 0, sizeof(PorkLauncherInterface));
    interface.DisplayMessageBox = IDisplayMessageBox;
    interface.DisplayViewport = IDisplayViewport;
    interface.ShutdownLauncher = IShutdownLauncher;

    InitPork(argc, argv, interface);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(0);

    return EXIT_SUCCESS;
}