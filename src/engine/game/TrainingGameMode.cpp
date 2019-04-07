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
#include "TrainingGameMode.h"

void TrainingGameMode::StartMode() {
    if(g_state.max_players > GetMaxPlayers()) {
        g_state.max_players = GetMaxPlayers();
    }

    // in this case, we'll forcefully load the training map
    ChangeMap("camp");
}

void TrainingGameMode::EndMode() {

}

void TrainingGameMode::StartTurn() {
    // todo
}

void TrainingGameMode::EndTurn() {
    // todo
}

void TrainingGameMode::StartRound() {
    // todo
}

void TrainingGameMode::RestartRound() {
    // todo
}

void TrainingGameMode::EndRound() {
    // todo
}

void TrainingGameMode::ChangeMap(const std::string &name) {

}
