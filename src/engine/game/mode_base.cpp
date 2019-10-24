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

#include "mode_base.h"
#include "actor_manager.h"
#include "player.h"

using namespace openhow;

BaseGameMode::BaseGameMode(const GameModeDescriptor& descriptor) {

}

BaseGameMode::~BaseGameMode() = default;

void BaseGameMode::StartRound() {
  if (HasRoundStarted()) {
    Error("Attempted to change map in the middle of a round, aborting!\n");
  }

  SpawnActors();

  // Play the deployment music
  Engine::Audio()->PlayMusic("music/track" + std::to_string(std::rand() % 4 + 27) + ".ogg");

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

#if 0
  if (active_actor_ != nullptr) {
    active_actor_->HandleInput();

    // temp: force the camera at the actor pos
    Camera* camera = Engine::GameManagerInstance()->GetCamera();
    camera->SetPosition(active_actor_->GetPosition());
    camera->SetAngles(active_actor_->GetAngles());
  }
#endif
}

void BaseGameMode::SpawnActors() {
  Map* map = Engine::Game()->GetCurrentMap();
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

void BaseGameMode::StartTurn(Player* player) {
  if(player->GetNumChildren() == 0) {
    LogWarn("No valid control target for player \"%s\"!\n", player->GetTeam()->name.c_str());
    return;
  }

  player->PossessChild(0);
}

void BaseGameMode::EndTurn(Player* player) {
  player->DepossessChild();

  // move onto the next player
  if (++player_slot_ >= rotation_.size()) {
    player_slot_ = 0;
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

/**
 * Maximum players allowed in this mode.
 * @return Number of players.
 */
unsigned int BaseGameMode::GetMaxPlayers() const {
  return 4;
}

void BaseGameMode::AssignActorToPlayer(Actor* target, Player* owner) {
  owner->AddChild(target);
}
