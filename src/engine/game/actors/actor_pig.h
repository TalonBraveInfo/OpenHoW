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
#include "actor_item.h"
#include "inventory.h"

class APig : public AAnimatedModel, public InventoryManager {
  ACTOR_IMPLEMENT_SUPER(AAnimatedModel)

 public:
  APig();
  ~APig() override;

  void HandleInput() override;
  void Tick() override;

  void SetClass(int pclass);
  int GetClass() { return pclass_; }

  void SetPersonality(int personality);
  int GetPersonality();

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

 protected:
 private:
  AItem* current_equiped_item_{nullptr};

  AudioSource* speech_{nullptr};

  const Team* team_{nullptr};

  unsigned int personality_{1};

  enum {
    CLASS_NONE = -1,
    CLASS_ACE,
    CLASS_LEGEND,
    CLASS_MEDIC,
    CLASS_COMMANDO,
    CLASS_SPY,
    CLASS_SNIPER,
    CLASS_SABOTEUR,
    CLASS_HEAVY,
    CLASS_GRUNT,
  } pclass_{CLASS_NONE};

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
