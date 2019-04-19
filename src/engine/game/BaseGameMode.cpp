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

#include "BaseGameMode.h"
#include "ActorManager.h"

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

    SpawnActors();
}

void BaseGameMode::RestartRound() {
    DestroyActors();
    SpawnActors();
}

void BaseGameMode::EndRound() {
    if(HasRoundStarted()) {
        Error("Attempted to unload map in the middle of a round, aborting!\n");
    }

    DestroyActors();

    delete current_map_;
    current_map_ = nullptr;
}

void BaseGameMode::Tick() {
    if(!HasRoundStarted()) {
        // still setting the game up...
        return;
    }

    ActorManager::GetInstance()->TickActors();

    if(players_[current_player_].input_target != nullptr) {
        players_[current_player_].input_target->HandleInput();
    }
}

void BaseGameMode::SpawnActors() {
    std::vector<MapSpawn> spawns = current_map_->GetSpawns();
    for(auto spawn : spawns) {
        Actor *actor = ActorManager::GetInstance()->SpawnActor(spawn.name);
        if(actor == nullptr) {
            continue;
        }

        actor->SetPosition(PLVector3(
                spawn.position[0],
                spawn.position[1],
                spawn.position[2]));
        // todo: assign player pigs etc., temp hack
        if(actor->class_name == "GR_ME") {
            players_[0].input_target = actor;
        }
    }
}

void BaseGameMode::DestroyActors() {
    ActorManager::GetInstance()->DestroyActors();
}

void BaseGameMode::StartTurn() {

}

void BaseGameMode::EndTurn() {
    // move onto the next player
    if(++current_player_ >= players_.size()) {
        current_player_ = 0;
    }
}
