#include "../../engine.h"

#include "../actor_manager.h"

#include "actor_airship.h"

REGISTER_ACTOR(airship, AAirship)

using namespace openhow;

AAirship::AAirship() : SuperClass() {
    noise_ = Engine::Audio()->CreateSource("audio/en_bip.wav", 1.0F, 1.0F, true);    
    SetModel("scenery/airship1");
    SetAngles( {180.0F, 0.0F, 0.0F} );
    ShowModel(true);
    noise_->StartPlaying();    
}

AAirship::~AAirship() {    
    // TODO: noise_ is currently creating an error when closing the game!
    //Engine::Audio()->DestorySource(noise_); //?
    delete noise_;    
}

void AAirship::Tick()
{
    noise_->SetPosition(this->position_);
    SuperClass::Tick();    
}