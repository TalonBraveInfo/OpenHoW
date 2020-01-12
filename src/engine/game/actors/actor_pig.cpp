/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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
#include "../../Map.h"

#include "../player.h"
#include "../actor_manager.h"

#include "actor_pig.h"

REGISTER_ACTOR( ac_me, APig )    // Ace
REGISTER_ACTOR( gr_me, APig )    // Grunt
REGISTER_ACTOR( hv_me, APig )
REGISTER_ACTOR( le_me, APig )    // Legend
REGISTER_ACTOR( me_me, APig )    // Medic
REGISTER_ACTOR( sa_me, APig )    // Saboteur
REGISTER_ACTOR( sb_me, APig )    // Commando
REGISTER_ACTOR( sn_me, APig )    // Sniper
REGISTER_ACTOR( sp_me, APig )    // Spy

using namespace openhow;

APig::APig() : SuperClass() {
	speech_ = Engine::Audio()->CreateSource();
}

APig::~APig() {
	delete speech_;
}

void APig::HandleInput() {
	SuperClass::HandleInput();
}

void APig::Tick() {
	SuperClass::Tick();

	// temp
	DropToFloor();

	// a dead piggy
	if ( GetHealth() <= 0 ) {
		if ( speech_->IsPlaying() ) {
			return;
		}

		// TODO: actor that produces explosion fx (AFXExplosion / effect_explosion) ?
		Engine::Audio()->PlayLocalSound( "audio/e_1.wav", GetPosition(), { 0, 0, 0 }, true );

		Actor* boots = ActorManager::GetInstance()->CreateActor( "boots" );
		boots->SetPosition( GetPosition() );
		boots->SetAngles( GetAngles() );
		boots->DropToFloor();

		// activate the boots (should begin smoke effect etc.)
		boots->Activate();

		// ensure our own destruction on next tick
		ActorManager::GetInstance()->DestroyActor( this );
		return;
	}

	PLVector3 forward = GetForward();
	PLVector3 nPosition = position_, nAngles = angles_;
	nPosition.x += input_forward * 100.0f * forward.x;
	nPosition.y += input_forward * 100.0f * forward.y;
	nPosition.z += input_forward * 100.0f * forward.z;

	aim_pitch_ += input_pitch * 2.0f;
	nAngles.y += input_yaw * 2.0f;

	// Clamp height based on current tile pos
	Map* map = Engine::Game()->GetCurrentMap();
	float height = map->GetTerrain()->GetHeight( { nPosition.x, nPosition.z } );
	if ( ( nPosition.y - 32.f ) < height ) {
		nPosition.y = height + 32.f;
	}

#define MAX_PITCH 89.f
	if ( aim_pitch_ < -MAX_PITCH ) aim_pitch_ = -MAX_PITCH;
	if ( aim_pitch_ > MAX_PITCH ) aim_pitch_ = MAX_PITCH;

	VecAngleClamp( &nAngles );

	position_ = nPosition;
	angles_ = nAngles;

	speech_->SetPosition( GetPosition() );
}

void APig::SetClass( unsigned int pclass ) {
	// TODO: setup default inventory

	SetHealth( 100 );

#if 0 // TODO: pass via json blob instead...
	switch (pclass_) {
	  default:


	  case CLASS_ACE:
		model_path = "pigs/ac_hi";
		break;
	  case CLASS_LEGEND:
		model_path = "pigs/le_hi";
		break;
	  case CLASS_MEDIC:
		model_path = "pigs/me_hi";
		break;
	  case CLASS_COMMANDO:
		model_path = "pigs/sb_hi";
		break;
	  case CLASS_SPY:
		model_path = "pigs/sp_hi";
		break;
	  case CLASS_SNIPER:
		model_path = "pigs/sn_hi";
		break;
	  case CLASS_SABOTEUR:
		model_path = "pigs/sa_hi";
		break;
	  case CLASS_HEAVY:
		model_path = "pigs/hv_hi";
		break;
	  case CLASS_GRUNT:

	}
#endif
}

void APig::SetPersonality( unsigned int personality ) {
	// TODO: ensure all the necessary sounds are cached...
}

void APig::SetPlayerOwner( Player* owner ) {
	IGameMode* mode = Engine::Game()->GetMode();
	mode->AssignActorToPlayer( this, owner );
}

const Player* APig::GetPlayerOwner() {
	return nullptr;
}

void APig::SetTeam( unsigned int team ) {
	team_ = team;

#if 0
	Player* player = Engine::Game()->GetPlayerByIndex(team);
	if(player == nullptr) {
	  ActorManager::GetInstance()->DestroyActor(this);
	  LogWarn("Failed to set team for pig!\n");
	  return;
	}

	Engine::Game()->GetMode()->AssignActorToPlayer(this, player);
#endif
}

void APig::Deserialize( const ActorSpawn& spawn ) {
	SuperClass::Deserialize( spawn );

	// Ensure pig is spawned up in the air for deployment
	Map* map = Engine::Game()->GetCurrentMap();
	SetPosition( { position_.GetValue().x, map->GetTerrain()->GetMaxHeight(), position_.GetValue().z } );

	SetHealth( 100 );
	SetModel( "pigs/ac_hi" ); // temp
	SetTeam( spawn.team );
	//SetClass(pig_class);

	// Create and equip our parachute, and then
	// link it to ensure it gets destroyed when we do
	parachute_ = dynamic_cast<AParachuteWeapon*>(ActorManager::GetInstance()->CreateActor( "weapon_parachute" ));
	if ( parachute_ == nullptr ) {
		Error( "Failed to create \"item_parachute\" actor, aborting!\n" );
	}

	LinkChild( parachute_ );
	parachute_->Deploy();
}

bool APig::Possessed( const Player* player ) {
	// TODO
	PlayVoiceSample( VoiceCategory::READY );

	return SuperClass::Possessed( player );
}

void APig::Depossessed( const Player* player ) {
	// TODO
	SuperClass::Depossessed( player );
}

void APig::Killed() {
	// TODO: queue me until camera focus, if valid (in some cases we'll insta die)
	//  Check if we're in water...

	PlayVoiceSample( VoiceCategory::DEATH );

	ClearItems();
}

/**
 * Play a randomised voice sample based on personality and team
 * @param category
 */
void APig::PlayVoiceSample( VoiceCategory category ) {
#if 0
	const Player* player = GetPlayerOwner();
	if(player == nullptr) {
	  return;
	}

	const Team* team = player->GetTeam();
	if(team == nullptr) {
	  return;
	}

	char path[32];
	snprintf(path, sizeof(path), "speech/eng/pig%0d/%0d%s%0d%0d.wav",
		// this is horrible, will need to revisit it...
		static_cast<int>(GetPersonality()),
		static_cast<int>(GetPersonality()),
		team->voice_set.c_str(),
		static_cast<int>(category),
		rand() % 6 + 1
		);

	const AudioSample* sample = Engine::Audio()->CacheSample(path);
	if(sample == nullptr) {
	  return;
	}

	speech_->SetSample(sample);
	speech_->StartPlaying();
#endif
}
