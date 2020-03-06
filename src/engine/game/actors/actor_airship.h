#pragma once

#include "actor_vehicle.h"

class AAirship : public AVehicle {
	IMPLEMENT_ACTOR( AAirship, AVehicle )

public:
	AAirship();
	~AAirship() override;

	void Tick() override;

	void Deserialize( const ActorSpawn &spawn ) override;

protected:
private:
	AudioSource *ambientSource{ nullptr };
};