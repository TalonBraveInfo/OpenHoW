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

#include "../engine.h"
#include "../frontend.h"

#include "BaseGameMode.h"
#include "ActorManager.h"

BaseGameMode::BaseGameMode() {
    players_.resize(4);
}
BaseGameMode::~BaseGameMode() = default;

void BaseGameMode::StartMode(const std::string &map_name) {
    FrontEnd_SetState(FE_MODE_LOADING);

    if(current_map_ != nullptr) {
        Error("Map already loaded or in dirty state, aborting!\n");
    }

    try {
        current_map_ = new Map(map_name);
    } catch(const std::runtime_error &e) {
        Error("Failed to load map, aborting!\n%s\n", e.what());
    }

    StartRound();
}

void BaseGameMode::EndMode() {
    delete current_map_;
    current_map_ = nullptr;

    /* todo: should go to end-game screen! */
    FrontEnd_SetState(FE_MODE_MAIN_MENU);
}

void BaseGameMode::StartRound() {
    if(HasRoundStarted()) {
        Error("Attempted to change map in the middle of a round, aborting!\n");
    }

    SpawnActors();

    /* todo: we should actually pause here and wait for user input
     *       otherwise players won't have time to read the loading screen */
    FrontEnd_SetState(FE_MODE_GAME);

    round_started_ = true;
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
}

void BaseGameMode::Tick() {
    if(!HasRoundStarted()) {
        // still setting the game up...
        return;
    }

    Actor* slave = GetCurrentPlayer()->input_target;
    if(slave != nullptr) {
        slave->HandleInput();

        /* temp: force the camera at the actor pos */
        g_state.camera->position = slave->GetPosition();
        g_state.camera->angles = slave->GetAngles();
    }

    ActorManager::GetInstance()->TickActors();
}

void BaseGameMode::SpawnActors() {
    std::vector<MapSpawn> spawns = current_map_->GetSpawns();
    for(auto spawn : spawns) {
        Actor* actor = ActorManager::GetInstance()->SpawnActor(spawn.name);
        if(actor == nullptr) {
            continue;
        }

        //actor->SetPosition(PLVector3(spawn.position[0], spawn.position[1], spawn.position[2]));
        //actor->SetAngles(PLVector3(spawn.angles[0], spawn.angles[1], spawn.angles[2]));

        // todo: assign player pigs etc., temp hack
        if(strcmp(spawn.name, "GR_ME") == 0) {
            players_[0].input_target = actor;
        }
    }
}

void BaseGameMode::DestroyActors() {
    ActorManager::GetInstance()->DestroyActors();
}

void BaseGameMode::StartTurn() {
    Player *player = GetCurrentPlayer();
    if(player->input_target == nullptr) {
        LogWarn("No valid control target for player \"%s\"!\n", player->name.c_str());
        EndTurn();
        return;
    }
}

void BaseGameMode::EndTurn() {
    // move onto the next player
    if(++current_player_ >= players_.size()) {
        current_player_ = 0;
    }
}

Player* BaseGameMode::GetCurrentPlayer() {
    return &players_[current_player_];
}
