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

#include "../../engine.h"
#include "../../graphics/display.h"
#include "../actor_manager.h"

#include "actor_item.h"
#include "actor_pig.h"
#include "actor_weapon.h"

using namespace openhow;

struct ItemSpawnIndex {

};

AItem::AItem() : SuperClass() {}
AItem::~AItem() = default;

void AItem::Deserialize(const ActorSpawn &spawn) {
  SuperClass::Deserialize(spawn);
}

void AItem::Equipped(Actor *other) {

}

void AItem::PickUp(Actor *other) {
  u_assert(other != nullptr);

  APig *pig = dynamic_cast<APig *>(other);
  if (pig == nullptr) {
    return;
  }

  // on pickup, spawn the necessary item we'll be
  // adding into the targets inventory
  switch (item_ident_) {
    default:break;
    case ITEM_WEAPON_TROTTER:break;
    case ITEM_WEAPON_KNIFE:break;
    case ITEM_WEAPON_BAYONET:break;
    case ITEM_WEAPON_SWORD:break;
    case ITEM_WEAPON_CATTLEPROD:break;
    case ITEM_WEAPON_PISTOL:break;
    case ITEM_WEAPON_RIFLE:break;
    case ITEM_WEAPON_RIFLE_BURST:break;
    case ITEM_WEAPON_MACHINE_GUN: {
      AWeapon *weapon = dynamic_cast<AWeapon *>(ActorManager::GetInstance()->CreateActor("weapon_machine_gun"));
      if (weapon == nullptr) {
        break;
      }

      pig->AddInventory(weapon);
      break;
    }
    case ITEM_WEAPON_HMG:break;
    case ITEM_WEAPON_SNIPER_RIFLE:break;
    case ITEM_WEAPON_SHOTGUN:break;
    case ITEM_WEAPON_FLAMETHROWER:break;
    case ITEM_WEAPON_ROCKET_LAUNCHER:break;
    case ITEM_WEAPON_GUIDED_MISSILE:break;
    case ITEM_MEDICINE_DART:break;
    case ITEM_TRANQ:break;
    case ITEM_GRENADE:break;
    case ITEM_CLUSTER_GRENADE:break;
    case ITEM_HX_GRENADE:break;
    case ITEM_ROLLER_GRENADE:break;
    case ITEM_CONFUSION_GAS:break;
    case ITEM_FREEZE_GAS:break;
    case ITEM_MADNESS_GAS:break;
    case ITEM_POISON_GAS:break;
    case ITEM_MORTAR:break;
    case ITEM_BAZOOKA:break;
    case ITEM_AIRBURST:break;
    case ITEM_SUPER_AIRBURST:break;
    case ITEM_MEDICINE_BALL:break;
    case ITEM_HOMING_MISSILE:break;
    case ITEM_MINE:break;
    case AntiPMine:break;
    case ITEM_TNT:break;
    case VehLongRangeShell:break;
    case VehShockShell:break;
    case Veh1000LBSShell:break;
    case VehFireShell:break;
    case VehGasShell:break;
    case VehMineShell:break;
    case VehHeavyMachineGun:break;
    case VehFlamethrower:break;
    case ITEM_VEHICLE_AIRBURST:break;
    case ITEM_VEHICLE_BAZOOKA:break;
    case ITEM_VEHICLE_MORTAR:break;
    case ITEM_JETPACK:break;
    case ITEM_SUICIDE:break;
    case ITEM_HEALINGHANDS:break;
    case ITEM_SELFHEAL:break;
    case ITEM_PICKPOCKET:break;
    case ITEM_SHOCKWAVE:break;
    case ITEM_SPECIALOPS:break;
    case ITEM_AIRSTRIKE:break;
    case FireRainAirstrike:break;
    case ITEM_ENTERVEHICLE:break;
    case ITEM_ENTERBUILDING:break;
    case ITEM_EJECTPIG:break;
    case ITEM_MAPVIEW:break;
    case ITEM_BINOCULARS:break;
    case ITEM_SKIPTURN:break;
    case ITEM_SURRENDER:break;
    case ITEM_SUPERTNT:break;
    case ITEM_HIDE:break;
    case ITEM_SUPERSHOTGUN:break;
    case ITEM_SHRAPNELGRENADE:break;
    case ITEM_GRENADELAUNCHER:break;
    case ITEM_RANDOM:break;
    case ITEM_HEALTH:break;
  }
}

PLTexture *AItem::GetInventoryIcon() {
  return Display_GetDefaultTexture();
}

/**
 * Whether or not this item can be selected from the inventory.
 * @return True if it's selectable, otherwise not.
 */
bool AItem::IsSelectable() {
  return false;
}
