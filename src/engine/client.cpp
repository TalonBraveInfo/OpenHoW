/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include <PL/platform_graphics_camera.h>

#include "engine.h"
#include "input.h"
#include "client.h"
#include "audio.h"
#include "frontend.h"

#include "script/script.h"

#include "graphics/display.h"

/************************************************************/

enum {
    CAMERA_MODE_DEFAULT,
    CAMERA_MODE_FREE,

    MAX_CAMERA_MODES
};

void Client_ProcessInput() {
    static double input_delay = 0;
    if(input_delay < g_state.sys_ticks) {
        input_delay = g_state.sys_ticks + 10;

        if (Input_GetKeyState('`')) {
            Console_Toggle();
            return;
        }
    }

    FE_ProcessInput();

    switch(cv_camera_mode->i_value) {
        default:break;

        case CAMERA_MODE_DEFAULT: {
            /* follow currently selected pig */
        } break;

#if 0
        case CAMERA_MODE_FREE: {
            if(Input_GetActionState(0, ACTION_MOVE_FORWARD)) {
                g_state.camera->position.x += 4.f;
            } else if(Input_GetActionState(0, ACTION_MOVE_BACKWARD)) {
                g_state.camera->position.x -= 4.f;
            }

            if(Input_GetButtonState(0, PORK_BUTTON_L1)) {
                g_state.camera->position.y += 4.f;
            } else if(Input_GetButtonState(0, PORK_BUTTON_R1)) {
                g_state.camera->position.y -= 4.f;
            }

            if(Input_GetButtonState(0, PORK_BUTTON_L2)) {
                g_state.camera->position.z += 4.f;
            } else if(Input_GetButtonState(0, PORK_BUTTON_R2)) {
                g_state.camera->position.z -= 4.f;
            }

            if(Input_GetActionState(0, ACTION_TURN_LEFT)) {
                g_state.camera->angles.z += 4.f;
            } else if(Input_GetActionState(0, ACTION_TURN_RIGHT)) {
                g_state.camera->angles.z -= 4.f;
            }
        } break;
#endif
    }
}

/************************************************************/

void Client_Simulate(void) {
    Client_ProcessInput();
    FE_Simulate();
}
