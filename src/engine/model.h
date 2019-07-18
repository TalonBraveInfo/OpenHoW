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

#define MAX_BONES 32

typedef struct Keyframe {
  PLVector3 transforms[10];
  PLQuaternion rotations[15];
} Keyframe;

typedef struct Animation {
  unsigned int id;
  const char *name;

  Keyframe *frames;
  unsigned int num_frames;
} Animation;

enum {
  BONE_INDEX_PELVIS,
  BONE_INDEX_SPINE,
  BONE_INDEX_HEAD,

  BONE_INDEX_UPPER_ARM_L,
  BONE_INDEX_LOWER_ARM_L,
  BONE_INDEX_HAND_L,

  BONE_INDEX_UPPER_ARM_R,
  BONE_INDEX_LOWER_ARM_R,
  BONE_INDEX_HAND_R,

  BONE_INDEX_UPPER_LEG_L,
  BONE_INDEX_LOWER_LEG_L,
  BONE_INDEX_FOOT_L,

  BONE_INDEX_UPPER_LEG_R,
  BONE_INDEX_LOWER_LEG_R,
  BONE_INDEX_FOOT_R,

  MAX_BONE_INDICES
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Animations

enum { // todo, switch over to using macros here
  //MCAP (in-game)
      ANI_RUN_NORMAL = 0,         // Run cycle (normal)
  ANI_RUN_WOUNDED1 = 1,       // Run cycle (wounded)
  ANI_RUN_WOUNDED2 = 2,       // Run cycle (more wounded)
  ANI_WALK_BACKWARD = 3,      // Walk cycle (backwards)
  ANI_TURN = 4,               // Turning on Spot
  ANI_SWIM1 = 5,              // Swimming
  ANI_SWIM2 = 6,              // Swimming like Rick
  ANI_ENTER = 7,              // Getting into Vehicles
  ANI_JUMP_START = 8,         // Jumping - Start
  ANI_JUMP_LOOP = 9,          // Jumping - Middle
  ANI_JUMP_END = 10,          // Jumping - End
  ANI_CLIMB = 11,             // Scramble
  ANI_EQUIP_HANDGUN = 12,     // Getting out Handgun
  ANI_EQUIP_RIFLE = 13,       // Getting out Rifle
  ANI_EQUIP_MG = 14,          // Getting out Machine gun
  ANI_EQUIP_HEAVY = 15,       // Getting out Heavy weapon
  ANI_EQUIP_PUNCH = 16,       // Getting out Punch
  ANI_EQUIP_THROW = 17,       // Getting out Grenade
  ANI_EQUIP_MEELE = 18,       // Getting out Sword / Knife
  ANI_USE_THROW = 19,         // Using Grenade
  ANI_USE_PUNCH = 20,         // Using Punch
  ANI_USE_MEELE = 21,         // Sword / Knife
  ANI_USE_BAYONET = 22,       // Bayonet
  ANI_USE_HANDGUN = 23,       // Aiming Handgun
  ANI_USE_RIFLE = 24,         // Aiming Rifle
  ANI_USE_MG = 25,            // Aiming Machine gun
  ANI_USE_HEAVY = 26,         // Aiming Heavy weapon
  ANI_IDLE1 = 27,             // Standing around cycle 1
  ANI_IDLE2 = 28,             // Standing around cycle 2
  ANI_HURT = 29,              // Very Wounded
  ANI_PROUD = 30,             // Lord Flash-Heart Pose
  ANI_LOOK = 31,              // Looking around
  ANI_STUPID = 32,            // Looking gormless
  ANI_FEAR = 33,              // Cowering
  ANI_CLEAN1 = 34,            // Brushoff 1
  ANI_CLEAN2 = 35,            // Brushoff 2
  ANI_CLEAN3 = 36,            // Brushoff 3
  ANI_SNEEZE = 37,            // Sneeze
  ANI_FALL = 38,              // Flying through air/falling
  ANI_BOUNCE = 39,            // Bouncing on B-Hind
  ANI_STANDUP = 40,           // Getting to feet
  ANI_CELEBRATE1 = 41,        // Celebration #1
  ANI_CELEBRATE2 = 42,        // Celebration #2
  ANI_CELEBRATE3 = 43,        // Celebration #3
  ANI_SALUTE = 44,            // Salute
  ANI_LOOK_BACK = 45,         // Look back
  ANI_THINK = 46,             // Thinking
  ANI_DEATH1 = 47,            // Dying #1
  ANI_DEATH2 = 48,            // Dying #2
  ANI_DEATH3 = 49,            // Dying #3
  ANI_DROWN = 50,             // Drowning
  ANI_IDLE_COLD = 51,         // Idle Cold
  ANI_IDLE_HOT = 52,          // Idle Hot
  ANI_USE_MINE = 53,          // Lay Mine
  ANI_USE_HEAL = 54,          // Heal
  ANI_USE_STEAL = 55,         // Pick pocket
  ANI_USE_AIRSHIP = 56,       // Air strike
  ANI_USE_SUICIDE = 57,       // Hari Kiri
  ANI_PARACHUTE = 58,         // Parachuting

  //FEMCAP (front-end)
      ANI_59 = 59,                //
  ANI_60 = 60,                //
  ANI_61 = 61,                //
  ANI_62 = 62,                //
  ANI_63 = 63,                //
  ANI_64 = 64,                //
  ANI_65 = 65,                //
  ANI_66 = 66,                //
  ANI_67 = 67,                //
  ANI_68 = 68,                //
  ANI_69 = 69,                //
  ANI_70 = 70,                //
  ANI_71 = 71,                //
  ANI_72 = 72,                //
  ANI_73 = 73,                //
  ANI_74 = 74,                //
  ANI_75 = 75,                //
  ANI_76 = 76,                //
  ANI_77 = 77,                //
  ANI_78 = 78,                //
  ANI_79 = 79,                //
  ANI_80 = 80,                //
  ANI_81 = 81,                //
  ANI_82 = 82,                //
  ANI_83 = 83,                //
  ANI_84 = 84,                //
  ANI_85 = 85,                //
  ANI_86 = 86,                //
  ANI_87 = 87,                //
  ANI_88 = 88,                //
  ANI_89 = 89,                //
  ANI_90 = 90,                //
  ANI_91 = 91,                //
  ANI_92 = 92,                //
};

#define MAX_ANIMATIONS 93

/* * * * * * * * * * * * * * * * * */

class ModelManager {
 public:
  static ModelManager *GetInstance() {
    if (instance_ == nullptr) {
      instance_ = new ModelManager();
    }

    return instance_;
  }

  static void DestroyInstance() {
    delete instance_;
    instance_ = nullptr;
  }

  PLModel *LoadModel(const std::string &path, bool abort_on_fail = false);
  void UnloadModel(PLModel *model);
  void ClearModelCache();

  PLModel *GetFallbackModel() {
    return fallback_;
  }

 protected:
 private:
  ModelManager();
  ~ModelManager();

  PLModel *fallback_{nullptr};

  std::unordered_map<std::string, PLModel *> cached_models_;

  static ModelManager *instance_;
};
