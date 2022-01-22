// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include "AVehicle.h"

class AAirship : public AVehicle {
	IMPLEMENT_ACTOR( AAirship, AVehicle )

public:
	AAirship();
	~AAirship() override;

	void Tick() override;

	void Deserialize( const ActorSpawn &spawn ) override;

	inline unsigned int GetMaxOccupants() const override { return 0; }
	inline const char *GetEngineIdleSound() const override { return "audio/en_bip.wav";	}

protected:
private:
	hei::Vector3 myDestination;
	float myDestinationTolerance{ 0.0f };
	float myTurnSpeed{ 0.0f };
	unsigned int myTurnFrames{ 0 };
};