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
#include "../audio.h"

#include "SPGameMode.h"
#include "ActorManager.h"

SPGameMode::SPGameMode() {
    players_.resize(4);
}

SPGameMode::~SPGameMode() {
    AudioManager::GetInstance()->FreeSources();
    AudioManager::GetInstance()->FreeSamples();

    DestroyActors();
}

void SPGameMode::StartRound() {
    if(HasRoundStarted()) {
        Error("Attempted to change map in the middle of a round, aborting!\n");
    }

    SpawnActors();

    round_started_ = true;
}

void SPGameMode::RestartRound() {
    DestroyActors();
    SpawnActors();
}

void SPGameMode::EndRound() {
    DestroyActors();
}

void SPGameMode::Tick() {
    if(!HasRoundStarted()) {
        // still setting the game up...
        return;
    }

    Actor* slave = GetCurrentPlayer()->input_target;
    if(slave != nullptr) {
        slave->HandleInput();

        // temp: force the camera at the actor pos
        g_state.camera->position = slave->GetPosition();
        g_state.camera->angles = slave->GetAngles();
    }

    ActorManager::GetInstance()->TickActors();
}

void SPGameMode::SpawnActors() {
    Map* map = GameManager::GetInstance()->GetCurrentMap();
    if(map == nullptr) {
        Error("Attempted to spawn actors without having loaded a map!\n");
    }

    std::vector<MapSpawn> spawns = map->GetSpawns();
    for(auto spawn : spawns) {
        Actor* actor = ActorManager::GetInstance()->SpawnMapActor(spawn.name);
        if(actor == nullptr) {
            continue;
        }

        actor->SetPosition(PLVector3(spawn.position[0], spawn.position[1], spawn.position[2]));
        actor->SetAngles(PLVector3(spawn.angles[0] / 360.f, spawn.angles[1] / 360.f, spawn.angles[2] / 360.f));

        // todo: assign player pigs etc., temp hack
        if(strcmp(spawn.name, "GR_ME") == 0) {
            players_[0].input_target = actor;
        }
    }
}

void SPGameMode::DestroyActors() {
    ActorManager::GetInstance()->DestroyActors();
}

void SPGameMode::StartTurn() {
    Player *player = GetCurrentPlayer();
    if(player->input_target == nullptr) {
        LogWarn("No valid control target for player \"%s\"!\n", player->name.c_str());
        EndTurn();
        return;
    }
}

void SPGameMode::EndTurn() {
    // move onto the next player
    if(++current_player_ >= players_.size()) {
        current_player_ = 0;
    }
}

void SPGameMode::PlayerJoined(Player *player) {

}

void SPGameMode::PlayerLeft(Player *player) {

}

unsigned int SPGameMode::GetMaxSpectators() const {
    return 0;
}

void SPGameMode::SpectatorJoined(Player *player) {

}

void SPGameMode::SpectatorLeft(Player *player) {

}

unsigned int SPGameMode::GetMaxPlayers() const {
    return 0;
}
