/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

enum class ItemIdentifier {
	NONE,

	WEAPON_TROTTER,
	WEAPON_KNIFE,
	WEAPON_BAYONET,
	WEAPON_SWORD,
	WEAPON_CATTLEPROD,
	WEAPON_PISTOL,
	WEAPON_RIFLE,
	WEAPON_RIFLE_BURST,
	WEAPON_MACHINE_GUN,
	WEAPON_HMG,
	WEAPON_SNIPER_RIFLE,
	WEAPON_SHOTGUN,
	WEAPON_FLAMETHROWER,
	WEAPON_ROCKET_LAUNCHER,
	WEAPON_GUIDED_MISSILE,
	MEDICINE_DART = 0x10,
	TRANQ = 0x11,
	GRENADE = 0x12,
	CLUSTER_GRENADE = 0x13,
	HX_GRENADE = 0x14,
	ROLLER_GRENADE = 0x15,
	CONFUSION_GAS = 0x16,
	FREEZE_GAS = 0x17,
	MADNESS_GAS = 0x18,
	POISON_GAS = 0x19,
	MORTAR = 0x1A,
	BAZOOKA = 0x1B,
	AIRBURST = 0x1C,
	SUPER_AIRBURST = 0x1D,
	MEDICINE_BALL = 0x1E,
	HOMING_MISSILE = 0x1F,
	MINE = 0x20,
	AntiPMine = 0x21,
	TNT = 0x22,
	VEHICLE_LongRangeShell = 35,
	VEHICLE_ShockShell,
	VEHICLE_1000LBSShell,
	VEHICLE_FireShell = 0x26,
	VEHICLE_GasShell = 0x27,
	VEHICLE_MineShell = 0x28,
	VEHICLE_HeavyMachineGun = 0x29,
	VEHICLE_Flamethrower = 0x2A,
	VEHICLE_AIRBURST = 0x2B,
	VEHICLE_BAZOOKA = 0x2C,
	VEHICLE_MORTAR = 0x2D,
	//Missing 0x2E
	//Missing 0x2F
	//Missing 0x30
	//Missing 0x31
	//Missing 0x32
	JETPACK = 0x33,
	SUICIDE = 0x34,
	HEALINGHANDS = 0x35,
	SELFHEAL = 0x36,
	PICKPOCKET = 0x37,
	SHOCKWAVE = 0x38,
	SPECIALOPS = 0x39,
	AIRSTRIKE = 0x3A,
	FireRainAirstrike = 0x3B,
	ENTERVEHICLE = 0x3C,
	ENTERBUILDING = 0x3D,
	EJECTPIG = 0x3E,
	MAPVIEW = 0x3F,
	BINOCULARS = 64,
	SKIPTURN,
	SURRENDER,
	SUPERTNT,
	HIDE,
	SUPERSHOTGUN,
	SHRAPNELGRENADE,
	GRENADELAUNCHER,
	//Missing 0x48
	RANDOM = 0x49,
	HEALTH = 0xFF,

	MAX_ITEM_TYPES
};

class InventoryItem {
public:
	InventoryItem() = default;
	~InventoryItem() = default;

	virtual void Equipped( Actor *actor );

	// Information for frontend
	virtual bool IsSelectable() { return false; }
	virtual std::string GetInventoryDescription() const { return "invalid"; }
	virtual PLTexture *GetInventoryIcon();

protected:
	unsigned int quantity_{ 0 };
	ItemIdentifier id_{ ItemIdentifier::NONE };

private:
};

class InventoryManager {
public:

	InventoryItem *GetItem( ItemIdentifier identifier );
	void AddInventoryItem( ItemIdentifier identifier, unsigned int quantity );
	void RemoveItem( ItemIdentifier identifier, unsigned int quantity );

	void ClearItems();

protected:
private:
	std::map<ItemIdentifier, InventoryItem *> items_;
};
