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

class AItem : public AModel {
  ACTOR_IMPLEMENT_SUPER(AModel)

 public:
  AItem();
  ~AItem() override;

  void Deserialize(const ActorSpawn& spawn) override;

  virtual void PickUp(Actor* other);

  // Information for frontend
  virtual std::string GetInventoryDescription() const { return "invalid"; }
  virtual PLTexture* GetInventoryIcon();

 protected:
  int item_quantity_{0};

  enum ItemIdent {
    ITEM_NONE,
    ITEM_WEAPON_TROTTER,
    ITEM_WEAPON_KNIFE,
    ITEM_WEAPON_BAYONET,
    ITEM_WEAPON_SWORD,
    ITEM_WEAPON_CATTLEPROD,
    ITEM_WEAPON_PISTOL,
    ITEM_WEAPON_RIFLE,
    ITEM_WEAPON_RIFLE_BURST,
    ITEM_WEAPON_MACHINE_GUN,
    ITEM_WEAPON_HMG,
    ITEM_WEAPON_SNIPER_RIFLE,
    ITEM_WEAPON_SHOTGUN,
    ITEM_WEAPON_FLAMETHROWER,
    ITEM_WEAPON_ROCKET_LAUNCHER,
    ITEM_WEAPON_GUIDED_MISSILE,
    ITEM_MEDICINE_DART = 0x10,
    ITEM_TRANQ = 0x11,
    ITEM_GRENADE = 0x12,
    ITEM_CLUSTER_GRENADE = 0x13,
    ITEM_HX_GRENADE = 0x14,
    ITEM_ROLLER_GRENADE = 0x15,
    ITEM_CONFUSION_GAS = 0x16,
    ITEM_FREEZE_GAS = 0x17,
    ITEM_MADNESS_GAS = 0x18,
    ITEM_POISON_GAS = 0x19,
    ITEM_MORTAR = 0x1A,
    ITEM_BAZOOKA = 0x1B,
    ITEM_AIRBURST = 0x1C,
    ITEM_SUPER_AIRBURST = 0x1D,
    ITEM_MEDICINE_BALL = 0x1E,
    ITEM_HOMING_MISSILE = 0x1F,
    ITEM_MINE = 0x20,
    AntiPMine = 0x21,
    ITEM_TNT = 0x22,
    VehLongRangeShell = 35,
    VehShockShell,
    Veh1000LBSShell,
    VehFireShell = 0x26,
    VehGasShell = 0x27,
    VehMineShell = 0x28,
    VehHeavyMachineGun = 0x29,
    VehFlamethrower = 0x2A,
    ITEM_VEHICLE_AIRBURST = 0x2B,
    ITEM_VEHICLE_BAZOOKA = 0x2C,
    ITEM_VEHICLE_MORTAR = 0x2D,
    //Missing 0x2E
    //Missing 0x2F
    //Missing 0x30
    //Missing 0x31
    //Missing 0x32
    ITEM_JETPACK = 0x33,
    ITEM_SUICIDE = 0x34,
    ITEM_HEALINGHANDS = 0x35,
    ITEM_SELFHEAL = 0x36,
    ITEM_PICKPOCKET = 0x37,
    ITEM_SHOCKWAVE = 0x38,
    ITEM_SPECIALOPS = 0x39,
    ITEM_AIRSTRIKE = 0x3A,
    FireRainAirstrike = 0x3B,
    ITEM_ENTERVEHICLE = 0x3C,
    ITEM_ENTERBUILDING = 0x3D,
    ITEM_EJECTPIG = 0x3E,
    ITEM_MAPVIEW = 0x3F,
    ITEM_BINOCULARS = 64,
    ITEM_SKIPTURN,
    ITEM_SURRENDER,
    ITEM_SUPERTNT,
    ITEM_HIDE,
    ITEM_SUPERSHOTGUN,
    ITEM_SHRAPNELGRENADE,
    ITEM_GRENADELAUNCHER,
    //Missing 0x48
    ITEM_RANDOM = 0x49,
    ITEM_HEALTH = 0xFF,
    MAX_ITEM_TYPES
  } item_ident_{ITEM_NONE};

 private:
};
