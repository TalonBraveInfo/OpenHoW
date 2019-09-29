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
#include "../Map.h"

#include "mode_base.h"
#include "actor_manager.h"
#include "actors/actor_pig.h"

using namespace openhow;

BaseGameMode::BaseGameMode() {
  players_.resize(4);
}

BaseGameMode::~BaseGameMode() = default;

void BaseGameMode::StartRound() {
  if (HasRoundStarted()) {
    Error("Attempted to change map in the middle of a round, aborting!\n");
  }

  SpawnActors();

  // Play the deployment music
  Engine::AudioManagerInstance()->PlayMusic("music/track" + std::to_string(std::rand() % 4 + 27) + ".ogg");

  round_started_ = true;
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
}

void BaseGameMode::SpawnActors() {
  Map* map = Engine::GameManagerInstance()->GetCurrentMap();
  if (map == nullptr) {
    Error("Attempted to spawn actors without having loaded a map!\n");
  }

  std::vector<ActorSpawn> spawns = map->GetSpawns();
  for (const auto& spawn : spawns) {
    Actor* actor = ActorManager::GetInstance()->CreateActor(spawn.class_name);
    if (actor == nullptr) {
      actor = ActorManager::GetInstance()->CreateActor("static_model");
    }

    actor->Deserialize(spawn);

    // Pigs are a special case, for obvious reasons
    APig* pig = dynamic_cast<APig*>(actor);
    if (pig == nullptr) {
      continue;
    }

    // add each pig to the players list
    for(auto& player : players_) {
      if(pig->GetTeam() == player.GetTeam()) {
      }
    }
  }

#if 0 // debug sprites...
  for(unsigned int i = 0; i < 4096; ++i) {
    Actor* actor = ActorManager::GetInstance()->CreateActor("sprite");
    if(actor == nullptr) {
      break;
    }

    actor->SetPosition({
                    static_cast<float>(rand() % TERRAIN_PIXEL_WIDTH),
                    static_cast<float>(rand() % TERRAIN_PIXEL_WIDTH),
                    static_cast<float>(rand() % TERRAIN_PIXEL_WIDTH)
                });
    actor->SetAngles({
                  static_cast<float>(rand() % TERRAIN_PIXEL_WIDTH),
                  static_cast<float>(rand() % TERRAIN_PIXEL_WIDTH),
                  static_cast<float>(rand() % TERRAIN_PIXEL_WIDTH)
              });
  }
#endif

  ActorManager::GetInstance()->ActivateActors();
}

void BaseGameMode::DestroyActors() {
  ActorManager::GetInstance()->DestroyActors();
}

void BaseGameMode::StartTurn() {
#if 0 // TODO: rethink...
  Player* player = GetCurrentPlayer();
  if (player->GetInputTarget() == nullptr) {
    LogWarn("No valid control target for player \"%s\"!\n", player->GetTeam()->name.c_str());
    EndTurn();
    return;
  }
#endif
}

void BaseGameMode::EndTurn() {
  // move onto the next player
  if (++current_player_ >= players_.size()) {
    current_player_ = 0;
  }
}

void BaseGameMode::PlayerJoined(Player* player) {
  // todo: display prompt
  LogInfo("%s has joined the game\n", player->GetTeam()->name.c_str());
}

void BaseGameMode::PlayerLeft(Player* player) {
  // todo: display prompt
  LogInfo("%s has left the game\n", player->GetTeam()->name.c_str());
}

unsigned int BaseGameMode::GetMaxSpectators() const {
  return 0;
}

void BaseGameMode::SpectatorJoined(Player* player) {
  // todo: display prompt
  LogInfo("%s has joined the spectators\n", player->GetTeam()->name.c_str());
}

void BaseGameMode::SpectatorLeft(Player* player) {
  // todo: display prompt
  LogInfo("%s has left the spectators\n", player->GetTeam()->name.c_str());
}

unsigned int BaseGameMode::GetMaxPlayers() const {
  return 4;
}
