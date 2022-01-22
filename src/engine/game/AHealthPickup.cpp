// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Menu.h"
#include "ActorManager.h"
#include "APig.h"

class AHealthPickup : public AModel {
	IMPLEMENT_ACTOR( AHealthPickup, AModel )

public:
	AHealthPickup();
	~AHealthPickup() override;

	void Touch( Actor *other ) override;

	bool IsVisibleOnMinimap() const override { return true; }
	unsigned int GetMinimapIconStyle() const override { return MINIMAP_ICON_HEALTH; }
	PLColour GetMinimapIconColour() const override { return hei::Colour( 255, 192, 203 ); }

protected:
private:
	unsigned int myPickupQuantity{ 0 };
};

REGISTER_ACTOR( crate2, AHealthPickup )

using namespace ohw;

AHealthPickup::AHealthPickup() : SuperClass() {}
AHealthPickup::~AHealthPickup() = default;

void AHealthPickup::Touch( Actor *other ) {
	SuperClass::Touch( other );

	APig *pig = dynamic_cast<APig *>(other);
	if ( pig == nullptr ) {
		// only pigs should be able to pick these up!
		return;
	}

	pig->AddHealth( myPickupQuantity );

	// may want to introduce networking logic here for actor destruction
	// hence why it's done this way for now
	ActorManager::GetInstance()->DestroyActor( this );
}
