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

#include "../engine.h"
#include "../script/script.h"
#include "../map.h"

#include "actor.h"

PL_EXTERN_C

void Server_Initialize(void) {
    LogInfo("initializing server...\n");
}

void Server_Simulate(void) {
    if(!g_state.is_host) {
        return;
    }

    ActorManager::GetInstance()->SimulateActors();

}

void Server_Shutdown(void) {
    LogInfo("shutting down server...\n");

    ActorManager::GetInstance()->ClearActors();

    Map_Unload();
}

PL_EXTERN_C_END
