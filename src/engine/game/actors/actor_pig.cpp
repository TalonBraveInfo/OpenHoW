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

#include "../../engine.h"
#include "../../Map.h"

#include "../actor_manager.h"

#include "actor_pig.h"

REGISTER_ACTOR(ac_me, APig)    // Ace
REGISTER_ACTOR(gr_me, APig)    // Grunt
REGISTER_ACTOR(hv_me, APig)
REGISTER_ACTOR(le_me, APig)    // Legend
REGISTER_ACTOR(me_me, APig)    // Medic
REGISTER_ACTOR(sa_me, APig)    // Saboteur
REGISTER_ACTOR(sb_me, APig)    // Commando
REGISTER_ACTOR(sn_me, APig)    // Sniper
REGISTER_ACTOR(sp_me, APig)    // Spy

using namespace openhow;

APig::APig() : SuperClass() {
  speech_ = Engine::AudioManagerInstance()->CreateSource();
}

APig::~APig() {
  delete speech_;
}

void APig::HandleInput() {
  SuperClass::HandleInput();
}

void APig::Tick() {
  SuperClass::Tick();

  // a dead piggy
  if(GetHealth() <= 0) {
    if(speech_->IsPlaying()) {
      return;
    }

    // TODO: actor that produces explosion fx (AFXExplosion / effect_explosion) ?
    Engine::AudioManagerInstance()->PlayLocalSound("audio/e_1.wav", GetPosition(), {0, 0, 0}, true);

    Actor* boots = ActorManager::GetInstance()->CreateActor("boots");
    boots->SetPosition(GetPosition());
    boots->SetAngles(GetAngles());
    boots->DropToFloor();

    // activate the boots (should begin smoke effect etc.)
    boots->Activate();

    // ensure our own destruction on next tick
    ActorManager::GetInstance()->DestroyActor(this);
    return;
  }

  Camera* camera = Engine::GameManagerInstance()->GetCamera();
  position_.x += input_forward * 100.0f * camera->GetForward().x;
  position_.y += input_forward * 100.0f * camera->GetForward().y;
  position_.z += input_forward * 100.0f * camera->GetForward().z;

  angles_.x += input_pitch * 2.0f;
  angles_.y += input_yaw * 2.0f;

  // Clamp height based on current tile pos
  Map* map = Engine::GameManagerInstance()->GetCurrentMap();
  float height = map->GetTerrain()->GetHeight(PLVector2(position_.x, position_.z));
  if ((position_.y - 32.f) < height) {
    position_.y = height + 32.f;
  }

#define MAX_PITCH 89.f
  if (angles_.x < -MAX_PITCH) angles_.x = -MAX_PITCH;
  if (angles_.x > MAX_PITCH) angles_.x = MAX_PITCH;

  VecAngleClamp(&angles_);
}

void APig::SetClass(int pclass) {
  std::string model_path;
  switch (pclass_) {
    default:
      LogWarn("Unknown pig class, \"%d\", destroying!\n");
      ActorManager::GetInstance()->DestroyActor(this);
      break;

    case CLASS_ACE:
      model_path = "pigs/ac_hi";
      break;
    case CLASS_LEGEND:
      model_path = "pigs/le_hi";
      break;
    case CLASS_MEDIC:
      model_path = "pigs/me_hi";
      break;
    case CLASS_COMMANDO:
      model_path = "pigs/sb_hi";
      break;
    case CLASS_SPY:
      model_path = "pigs/sp_hi";
      break;
    case CLASS_SNIPER:
      model_path = "pigs/sn_hi";
      break;
    case CLASS_SABOTEUR:
      model_path = "pigs/sa_hi";
      break;
    case CLASS_HEAVY:
      model_path = "pigs/hv_hi";
      break;
    case CLASS_GRUNT:
      model_path = "pigs/gr_hi";
      break;
  }

  if (!model_path.empty()) {
    SetModel(model_path);
  }
}

void APig::Deserialize(const ActorSpawn& spawn) {
  SuperClass::Deserialize(spawn);

  // ensure pig is spawned up in the air for deployment
  Map* map = Engine::GameManagerInstance()->GetCurrentMap();
  SetPosition({position_.x, map->GetTerrain()->GetMaxHeight(), position_.z});

  // TODO: this is slightly more complicated...
  SetClass(spawn.appearance);

  AddItem(dynamic_cast<AItem*>(ActorManager::GetInstance()->CreateActor("item_parachute")));
}

bool APig::Possessed(const Player* player) {
  // TODO
  //  Update camera
  PlayVoiceSample(VoiceCategory::READY);

  return SuperClass::Possessed(player);
}

void APig::Depossessed(const Player* player) {
  // TODO
  SuperClass::Depossessed(player);
}

void APig::Killed() {
  // TODO

  ClearItems();
}

/**
 * Play a randomised voice sample based on personality and team
 * @param category
 */
void APig::PlayVoiceSample(VoiceCategory category) {

}
