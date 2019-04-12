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
#include "../Map.h"
#include "BaseGameMode.h"

BaseGameMode::~BaseGameMode() = default;

void BaseGameMode::StartMode() {
}

void BaseGameMode::EndMode() {
}

void BaseGameMode::StartRound(const std::string &map_name) {
    if(HasRoundStarted()) {
        Error("Attempted to change map in the middle of a round, aborting!\n");
    }

    if(current_map_ != nullptr) {
        Error("Map already loaded in or dirty state, aborting!\n");
    }

    try {
        current_map_ = new Map(map_name);
    } catch(const std::runtime_error &e) {
        Error("Failed to load map, aborting!\n%s\n", e.what());
    }


}

void BaseGameMode::EndRound() {
    if(current_map_ == nullptr) {
        return;
    }

    if(HasRoundStarted()) {
        Error("Attempted to unload map in the middle of a round, aborting!\n");
    }

    delete current_map_;
}

void BaseGameMode::Tick() {
    if(!HasRoundStarted()) {
        // still setting the game up...
        return;
    }


}

void BaseGameMode::SpawnActors() {

}

void BaseGameMode::DestroyActors() {

}
