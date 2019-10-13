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

#pragma once

#include "actor_model.h"
#include "actor_animated_model.h"
#include "actor_weapon.h"
#include "item_parachute.h"
#include "../inventory.h"

enum class PigPersonality {
  NONE = -1,
};

enum class PigClass {
  NONE = -1,
  GRUNT,
  // Heavy Weapons
  GUNNER,
  BOMBARDIER,
  PYROTECHNIC,
  // Engineer
  SAPPER,
  ENGINEER,
  SABOTEUR,
  // Espionage
  SCOUT,
  SNIPER,
  SPY,
  // Medic
  ORDERLY,
  MEDIC,
  SURGEON,
  // High Rank
  COMMANDO,
  HERO,
  ACE,
  LEGEND,
  // Multiplayer
  PARATROOPER,
  GRENADIER,
};

class APig : public AAnimatedModel, public InventoryManager {
  ACTOR_IMPLEMENT_SUPER(AAnimatedModel)

 public:
  APig();
  ~APig() override;

  void HandleInput() override;
  void Tick() override;

  void SetClass(int pclass);
  PigClass GetClass() { return class_; }

  void SetPersonality(PigPersonality personality);
  PigPersonality GetPersonality() { return personality_; }

  void SetTeam(const Team* team);
  const Team* GetTeam() const { return team_; }

  bool Possessed(const Player* player) override;
  void Depossessed(const Player* player) override;

  enum class VoiceCategory {
    READY,
    FIRE,
    FIRE2,
    DEATH,
    DEATH2
  };
  void PlayVoiceSample(VoiceCategory category);

  void Killed();

  void Deserialize(const ActorSpawn& spawn) override;

 private:
  AWeapon* weapon_{nullptr};
  AParachuteItem* parachute_{nullptr};

  AudioSource* speech_{nullptr};

  const Team* team_{nullptr};

  PigPersonality personality_{ PigPersonality::NONE };
  PigClass class_{ PigClass::NONE };

  enum {
    EYES_OPEN,
    EYES_CLOSED,
    EYES_SAD,
    EYES_SURPRISED,
    EYES_ANGRY,
  } upper_face_frame_{EYES_OPEN};

  enum {
    MOUTH_OPEN,
    MOUTH_CLOSED,
    MOUTH_SAD,
    MOUTH_SURPRISED,
    MOUTH_ANGRY,
  } lower_face_frame_{MOUTH_OPEN};
};
