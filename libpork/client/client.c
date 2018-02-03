/* OpenHoW
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
#include <PL/platform_graphics_camera.h>

#include "pork_engine.h"
#include "pork_input.h"
#include "pork_console.h"

#include "client.h"

/*****************************************************/

void ProcessClientInput(void) {
    static double input_delay = 0;
    if(input_delay < g_state.sim_ticks) {
        input_delay = g_state.sim_ticks + 50;

        if (GetKeyState('`')) {
            ToggleConsole();
            return;
        }
    }

#if 1
    if(GetActionState(0, ACTION_MOVE_FORWARD)) {
        g_state.camera->position.x += 5.f;
    } else if(GetActionState(0, ACTION_MOVE_BACKWARD)) {
        g_state.camera->position.x -= 5.f;
    }

    if(GetActionState(0, ACTION_MOVE_LEFT)) {

    } else if(GetActionState(0, ACTION_MOVE_RIGHT)) {

    }
#endif
}

/*****************************************************/

void SimulateClient(void) {
    ProcessClientInput();
}
