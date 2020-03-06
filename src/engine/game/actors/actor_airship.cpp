#include "../../engine.h"

#include "../actor_manager.h"

#include "actor_airship.h"

REGISTER_ACTOR( airship, AAirship )

using namespace openhow;

AAirship::AAirship() : SuperClass() {}

AAirship::~AAirship() {
	delete ambientSource;
}

void AAirship::Tick() {
	SuperClass::Tick();

	ambientSource->SetPosition( GetPosition() );
}

void AAirship::Deserialize( const ActorSpawn &spawn ) {
	SuperClass::Deserialize( spawn );

	ambientSource = Engine::Audio()->CreateSource( "audio/en_bip.wav",
												   { 0.0f, 0.0f, 0.0f },
												   { 0.0f, 0.0f, 0.0f },
												   true,
												   1.0f,
												   1.0f,
												   true );
	ambientSource->StartPlaying();

	SetModel( "scenery/airship1" );
	SetAngles( { 180.0f, 0.0f, 0.0f } );
	ShowModel( true );
}
