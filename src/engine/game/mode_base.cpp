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
#include "../audio/audio.h"

#include "mode_base.h"
#include "ActorManager.h"
#include "actors/actor_pig.h"

BaseGameMode::BaseGameMode() {
  players_.resize(4);
}

BaseGameMode::~BaseGameMode() {
  AudioManager::GetInstance()->FreeSources();
  AudioManager::GetInstance()->FreeSamples();

  DestroyActors();
}

void BaseGameMode::StartRound() {
  if (HasRoundStarted()) {
    Error("Attempted to change map in the middle of a round, aborting!\n");
  }

  SpawnActors();

  round_started_ = true;

  // Play the deployment music
  AudioManager::GetInstance()->PlayMusic("music/track" + std::to_string(std::rand() % 4 + 27) + ".ogg");
}

void BaseGameMode::RestartRound() {
  DestroyActors();
  SpawnActors();
}

void BaseGameMode::EndRound() {
  DestroyActors();
}

void BaseGameMode::Tick() {
  if (!HasRoundStarted()) {
    // still setting the game up...
    return;
  }

  Actor* slave = GetCurrentPlayer()->input_target;
  if (slave != nullptr) {
    slave->HandleInput();

    // temp: force the camera at the actor pos
    g_state.camera->position = slave->GetPosition();
    g_state.camera->angles = slave->GetAngles();
  }

  ActorManager::GetInstance()->TickActors();
}

void BaseGameMode::SpawnActors() {
  Map* map = GameManager::GetInstance()->GetCurrentMap();
  if (map == nullptr) {
    Error("Attempted to spawn actors without having loaded a map!\n");
  }

  std::vector<ActorSpawn> spawns = map->GetSpawns();
  for (const auto& spawn : spawns) {
    LogInfo("Spawn: %s\n", spawn.class_name.c_str());

    Actor* actor = ActorManager::GetInstance()->CreateActor(spawn);
    if (actor == nullptr) {
      continue;
    }

    // Pigs are a special case, for obvious reasons
    APig* pig = dynamic_cast<APig*>(actor);
    if (pig == nullptr) {
      continue;
    }

    players_[0].input_target = actor;
  }
}

void BaseGameMode::DestroyActors() {
  ActorManager::GetInstance()->DestroyActors();
}

void BaseGameMode::StartTurn() {
  Player* player = GetCurrentPlayer();
  if (player->input_target == nullptr) {
    LogWarn("No valid control target for player \"%s\"!\n", player->name.c_str());
    EndTurn();
    return;
  }
}

void BaseGameMode::EndTurn() {
  // move onto the next player
  if (++current_player_ >= players_.size()) {
    current_player_ = 0;
  }
}

void BaseGameMode::PlayerJoined(Player* player) {

}

void BaseGameMode::PlayerLeft(Player* player) {

}

unsigned int BaseGameMode::GetMaxSpectators() const {
  return 0;
}

void BaseGameMode::SpectatorJoined(Player* player) {

}

void BaseGameMode::SpectatorLeft(Player* player) {

}

unsigned int BaseGameMode::GetMaxPlayers() const {
  return 0;
}
