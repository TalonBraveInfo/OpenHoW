#pragma once

#include "actor_vehicle.h"

class AAirship : public AVehicle {
    IMPLEMENT_ACTOR(AAirship, AVehicle)

public:
    AAirship();
    ~AAirship() override;

    void Tick() override;


protected:
private:    
    AudioSource* noise_{nullptr};
};