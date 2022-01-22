// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Menu.h"
#include "ActorManager.h"
#include "Inventory.h"
#include "APig.h"

class AItemPickup : public AModel {
	IMPLEMENT_ACTOR( AItemPickup, AModel )

public:
	AItemPickup();
	~AItemPickup() override;

	void Touch( Actor *other ) override;

	bool IsVisibleOnMinimap() const override { return true; }
	unsigned int GetMinimapIconStyle() const override { return MINIMAP_ICON_PICKUP; }
	PLColour GetMinimapIconColour() const override { return hei::Colour( 210, 105, 30 ); }

private:
	ItemIdentifier myPickupId{ ItemIdentifier::NONE };
	unsigned int myPickupQuantity{ 0 };
};

AItemPickup::AItemPickup() : SuperClass() {}
AItemPickup::~AItemPickup() = default;

void AItemPickup::Touch( Actor *other ) {
	SuperClass::Touch( other );

	APig *pig = dynamic_cast<APig *>(other);
	if ( pig == nullptr ) {
		// only pigs should be able to pick these up!
		return;
	}

	pig->AddInventoryItem( myPickupId, myPickupQuantity );

#if 0
	// On pickup, we'll need to pass the item into
	// the pig's inventory
	switch(pickup_id_) {
	  default:break;
	  case ItemId::WEAPON_TROTTER:break;
	  case ItemId::WEAPON_KNIFE:break;
	  case ItemId::WEAPON_BAYONET:break;
	  case ItemId::WEAPON_SWORD:break;
	  case ItemId::WEAPON_CATTLEPROD:break;
	  case ItemId::WEAPON_PISTOL:break;
	  case ItemId::WEAPON_RIFLE:break;
	  case ItemId::WEAPON_RIFLE_BURST:break;
	  case ItemId::WEAPON_MACHINE_GUN:break;
	  case ItemId::WEAPON_HMG:break;
	  case ItemId::WEAPON_SNIPER_RIFLE:break;
	  case ItemId::WEAPON_SHOTGUN:break;
	  case ItemId::WEAPON_FLAMETHROWER:break;
	  case ItemId::WEAPON_ROCKET_LAUNCHER:break;
	  case ItemId::WEAPON_GUIDED_MISSILE:break;
	  case ItemId::MEDICINE_DART:break;
	  case ItemId::TRANQ:break;
	  case ItemId::GRENADE:break;
	  case ItemId::CLUSTER_GRENADE:break;
	  case ItemId::HX_GRENADE:break;
	  case ItemId::ROLLER_GRENADE:break;
	  case ItemId::CONFUSION_GAS:break;
	  case ItemId::FREEZE_GAS:break;
	  case ItemId::MADNESS_GAS:break;
	  case ItemId::POISON_GAS:break;
	  case ItemId::MORTAR:break;
	  case ItemId::BAZOOKA:break;
	  case ItemId::AIRBURST:break;
	  case ItemId::SUPER_AIRBURST:break;
	  case ItemId::MEDICINE_BALL:break;
	  case ItemId::HOMING_MISSILE:break;
	  case ItemId::MINE:break;
	  case ItemId::AntiPMine:break;
	  case ItemId::TNT:break;
	  case ItemId::VEHICLE_LongRangeShell:break;
	  case ItemId::VEHICLE_ShockShell:break;
	  case ItemId::VEHICLE_1000LBSShell:break;
	  case ItemId::VEHICLE_FireShell:break;
	  case ItemId::VEHICLE_GasShell:break;
	  case ItemId::VEHICLE_MineShell:break;
	  case ItemId::VEHICLE_HeavyMachineGun:break;
	  case ItemId::VEHICLE_Flamethrower:break;
	  case ItemId::VEHICLE_AIRBURST:break;
	  case ItemId::VEHICLE_BAZOOKA:break;
	  case ItemId::VEHICLE_MORTAR:break;
	  case ItemId::JETPACK:break;
	  case ItemId::SUICIDE:break;
	  case ItemId::HEALINGHANDS:break;
	  case ItemId::SELFHEAL:break;
	  case ItemId::PICKPOCKET:break;
	  case ItemId::SHOCKWAVE:break;
	  case ItemId::SPECIALOPS:break;
	  case ItemId::AIRSTRIKE:break;
	  case ItemId::FireRainAirstrike:break;
	  case ItemId::ENTERVEHICLE:break;
	  case ItemId::ENTERBUILDING:break;
	  case ItemId::EJECTPIG:break;
	  case ItemId::MAPVIEW:break;
	  case ItemId::BINOCULARS:break;
	  case ItemId::SKIPTURN:break;
	  case ItemId::SURRENDER:break;
	  case ItemId::SUPERTNT:break;
	  case ItemId::HIDE:break;
	  case ItemId::SUPERSHOTGUN:break;
	  case ItemId::SHRAPNELGRENADE:break;
	  case ItemId::GRENADELAUNCHER:break;
	  case ItemId::RANDOM:break;
	  case ItemId::HEALTH:break;
	}
#endif

	// may want to introduce networking logic here for actor destruction
	// hence why it's done this way for now
	ActorManager::GetInstance()->DestroyActor( this );
}
