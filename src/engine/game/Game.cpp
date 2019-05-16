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

static BaseGameMode* current_mode = nullptr;

bool Game_SetMode(const std::string &mode) {
    LogDebug("starting new game...\n");

    if(current_mode != nullptr) {
        if (mode == current_mode->GetDescription()) {
            return true;
        }

        if (current_mode->HasRoundStarted() || current_mode->HasTurnStarted()) {
            Error("Attempted to change mode in the middle of a round!\n");
        }

        delete current_mode;
        current_mode = nullptr;
    }

    /* todo:
     *  eventually this will operate through a list
     *  of modes registered to a parent class and select
     *  it by that, rather than this hard-coded method.
     */
    if(mode == "singleplayer") {
        current_mode = new BaseGameMode();
    } else if(mode == "training") {
        current_mode = new TrainingGameMode();
    } else {
        LogWarn("Unknown game-mode specified, \"%s\"!\n", mode.c_str());
        return false;
    }

    return true;
}

BaseGameMode *Game_GetMode() {
    return current_mode;
}
