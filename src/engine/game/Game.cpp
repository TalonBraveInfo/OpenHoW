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

static BaseGameMode *mode = nullptr;
const BaseGameMode *Game_GetMode() {
    return mode;
}

void Game_SetMode(const std::string &mode_desc) {
    LogDebug("starting new game...\n");

    if(mode != nullptr) {
        mode->EndMode();
        delete mode;
    }

    if(mode_desc == "training") {
        //mode = new TrainingGameMode();
    } else {
        Error("Unknown game-mode specified, \"%s\"!\n", mode_desc.c_str());
    }

    mode->StartMode();
}

