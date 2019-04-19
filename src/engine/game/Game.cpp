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

#include "Game.h"
#include "BaseGameMode.h"
#include "TrainingGameMode.h"

bool SetGameMode(const std::string &mode) {
    LogDebug("starting new game...\n");

    if(g_state.mode != nullptr) {
        if (mode == g_state.mode->GetDescription()) {
            return true;
        }

        if (g_state.mode->HasRoundStarted() || g_state.mode->HasTurnStarted()) {
            Error("Attempted to change mode in the middle of a round!\n");
        }

        delete g_state.mode;
        g_state.mode = nullptr;
    }

    /* todo:
     *  eventually this will operate through a list
     *  of modes registered to a parent class and select
     *  it by that, rather than this hard-coded method.
     */
    if(mode == "singleplayer") {
        g_state.mode = new BaseGameMode();
    } else if(mode == "training") {
        g_state.mode = new TrainingGameMode();
    } else {
        LogWarn("Unknown game-mode specified, \"%s\"!\n", mode.c_str());
        return false;
    }

    return true;
}

