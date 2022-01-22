// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Menu.h"
#include "Map.h"
#include "Player.h"
#include "ActorManager.h"
#include "APig.h"

REGISTER_ACTOR( ac_me, APig )    // Ace
REGISTER_ACTOR( gr_me, APig )    // Grunt
REGISTER_ACTOR( hv_me, APig )
REGISTER_ACTOR( le_me, APig )    // Legend
REGISTER_ACTOR( me_me, APig )    // Medic
REGISTER_ACTOR( sa_me, APig )    // Saboteur
REGISTER_ACTOR( sb_me, APig )    // Commando
REGISTER_ACTOR( sn_me, APig )    // Sniper
REGISTER_ACTOR( sp_me, APig )    // Spy

REGISTER_ACTOR_BASIC( APig )

using namespace ohw;

APig::APig() : SuperClass() {
	speechEmitter = GetApp()->audioManager->CreateSource();
}

APig::~APig() {
	delete speechEmitter;
}

void APig::HandleInput() {
	IGameMode *mode = GetApp()->gameManager->GetMode();
	if ( mode == nullptr ) {
		return;
	}

	Player *player = mode->GetCurrentPlayer();
	if ( player == nullptr ) {
		return;
	}

	if ( GetApp()->inputManager->GetActionState( player->GetControllerSlot(), InputManager::ACTION_JUMP ) ) {
		Jump();
		return;
	}

	PLVector2 cl;
	GetApp()->inputManager->GetJoystickState( player->GetControllerSlot(), InputManager::JOYSTICK_AXIS_LEFT_STICK, &cl.x, &cl.y );
	if ( GetApp()->inputManager->GetActionState( player->GetControllerSlot(), InputManager::ACTION_MOVE_FORWARD ) ) {
		forwardVelocity = 1.0f;
	} else if ( GetApp()->inputManager->GetActionState( player->GetControllerSlot(), InputManager::ACTION_MOVE_BACKWARD ) ) {
		forwardVelocity = -1.0f;
	} else {
		forwardVelocity = -cl.y / 327.0f;
	}

	PLVector2 cr;
	GetApp()->inputManager->GetJoystickState( player->GetControllerSlot(), InputManager::JOYSTICK_AXIS_RIGHT_STICK, &cr.x, &cr.y );
	if ( GetApp()->inputManager->GetActionState( player->GetControllerSlot(), InputManager::ACTION_TURN_LEFT ) ) {
		inputYaw = -1.0f;
	} else if ( GetApp()->inputManager->GetActionState( player->GetControllerSlot(), InputManager::ACTION_TURN_RIGHT ) ) {
		inputYaw = 1.0f;
	} else {
		inputYaw = cr.x / 327.0f;
	}

	if ( GetApp()->inputManager->GetActionState( player->GetControllerSlot(), InputManager::ACTION_AIM_UP ) ) {
		inputPitch = 1.0f;
	} else if ( GetApp()->inputManager->GetActionState( player->GetControllerSlot(), InputManager::ACTION_AIM_DOWN ) ) {
		inputPitch = -1.0f;
	} else {
		inputPitch = -cr.y / 327.0f;
	}
}

void APig::Tick() {
	SuperClass::Tick();

	//float speedModifier = 1.0f;
	if ( !IsOnGround() ) {
		if ( parachuteWeapon->IsDeployed() ) {
			//speedModifier = 10.0f;
		}

		velocity.y = -75.0f;
	} else {
		// If we've landed, put it away
		if ( parachuteWeapon->IsDeployed() ) {
			parachuteWeapon->Holster();
		}
	}

	// a dead piggy
	if ( lifeState == LifeState::DEAD ) {
		return;
	} else if ( lifeState == LifeState::DYING ) {
		if ( speechEmitter->IsPlaying() ) {
			return;
		}

		// TODO: actor that produces explosion fx (AFXExplosion / effect_explosion) ?
		GetApp()->audioManager->PlayLocalSound( "audio/e_1.wav", GetPosition(), PLVector3(), true );

		SetModel( "scenery/boots.vtx" );
		DropToFloor();

		// todo; create particle emitter and link as child to the boots!
		lifeState = LifeState::DEAD;
		return;
	}

	// Update position

	hei::Vector3 nPosition = GetPosition();

	PLVector3 forwardDirection;
	forwardDirection.x = forwardVelocity * 50.0f * myForward.x;
	forwardDirection.y = forwardVelocity * 50.0f * myForward.y;
	forwardDirection.z = forwardVelocity * 50.0f * myForward.z;
	nPosition += velocity + forwardDirection;

	// Clamp height based on current tile pos
	Map *map = GetApp()->gameManager->GetCurrentMap();
	float height = map->GetTerrain()->GetHeight( nPosition.x, nPosition.z );
	if ( ( nPosition.y - ( boundingBox.maxs.y / 2 ) ) < height ) {
		nPosition.y = height + ( boundingBox.maxs.y / 2 );
	}

	SetPosition( nPosition );
	speechEmitter->SetPosition( nPosition );

	// Update angles

	aimPitch += inputPitch * 2.0f;
#define MAX_PITCH 89.f
	if ( aimPitch < -MAX_PITCH ) aimPitch = -MAX_PITCH;
	if ( aimPitch > MAX_PITCH ) aimPitch = MAX_PITCH;

	hei::Vector3 nAngles = GetAngles();
	nAngles.y += inputYaw * 2.0f;
	VecAngleClamp( &nAngles );

	SetAngles( nAngles );
}

void APig::SetClass( const std::string &classIdentifer ) {
	const CharacterClass *characterClass = GetApp()->gameManager->GetDefaultClass( classIdentifer );
	if ( characterClass == nullptr ) {
		Warning( "Failed to fetch valid character class for pig!\n" );
		return;
	}

	SetHealth( characterClass->health );
	SetModel( characterClass->model );
}

void APig::SetPersonality( unsigned int personality ) {
	// TODO: ensure all the necessary sounds are cached...
}

void APig::SetPlayerOwner( Player *owner ) {
	IGameMode *mode = GetApp()->gameManager->GetMode();
	if ( mode == nullptr ) {
		Warning( "Attempted to set player owner without an active mode!\n" );
		return;
	}

	mode->AssignActorToPlayer( this, owner );
	playerOwnerPtr = owner;
}

const Player *APig::GetPlayerOwner() const {
	return playerOwnerPtr;
}

void APig::SetTeam( unsigned int team ) {
	myTeam = team;

#if 0
	Player* player = GetApp()->gameManager->GetPlayerByIndex(team);
	if(player == nullptr) {
	  ActorManager::GetInstance()->DestroyActor(this);
	  LogWarn("Failed to set team for pig!\n");
	  return;
	}

	GetApp()->gameManager->GetMode()->AssignActorToPlayer(this, player);
#endif
}

void APig::Deserialize( const ActorSpawn &spawn ) {
	SuperClass::Deserialize( spawn );

	// Ensure pig is spawned up in the air for deployment
	Map *map = GetApp()->gameManager->GetCurrentMap();
	SetPosition( { position_.GetValue().x, map->GetTerrain()->GetMaxHeight(), position_.GetValue().z } );

	SetClass( spawn.className );
	SetTeam( spawn.team );

	// Create and equip our parachute, and then
	// link it to ensure it gets destroyed when we do
	parachuteWeapon = dynamic_cast<AParachuteWeapon *>(ActorManager::GetInstance()->CreateActor( "weapon_parachute" ));
	if ( parachuteWeapon == nullptr ) {
		Error( "Failed to create \"weapon_parachute\" actor, aborting!\n" );
	}

	LinkChild( parachuteWeapon );
	parachuteWeapon->SetPosition( GetPosition() );
	parachuteWeapon->Deploy();
}

bool APig::Possessed( const Player *player ) {
	// TODO
	PlayVoiceSample( VoiceCategory::READY );

	return SuperClass::Possessed( player );
}

void APig::Dispossessed(const Player *player ) {
	// TODO
	SuperClass::Dispossessed(player);
}

bool APig::IsVisibleOnMinimap() const {
	// TODO: handle special cases
	return true;
}

unsigned int APig::GetMinimapIconStyle() const {
	return MINIMAP_ICON_PIG;
}

PLColour APig::GetMinimapIconColour() const {
	const Player *player = GetPlayerOwner();
	if ( player == nullptr ) {
		return Actor::GetMinimapIconColour();
	}

	const PlayerTeam *team = player->GetTeam();
	if ( team == nullptr ) {
		return Actor::GetMinimapIconColour();
	}

	return team->colour;
}

void APig::Killed() {
	// TODO: queue me until camera focus, if valid (in some cases we'll insta die)
	//  Check if we're in water...
	if ( lifeState != LifeState::ALIVE ) {
		return;
	}

	PlayVoiceSample( VoiceCategory::DEATH );

	ClearItems();

	lifeState = LifeState::DYING;
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

	const AudioSample* sample = GetApp()->audioManager->CacheSample(path);
	if(sample == nullptr) {
	  return;
	}

	speechEmitter->SetSample(sample);
	speechEmitter->StartPlaying();
#endif
}

/**
 * Lift that pig into the air like you just don't care!
 */
void APig::Jump() {
	DebugMsg( "Jumping...\n" );

	// Make sure we can't jump again if we're already off the ground!
	if ( !IsOnGround() ) {
		return;
	}

	GetApp()->audioManager->PlayLocalSound( "audio/p_snort1.wav", GetPosition(), GetVelocity(), true );

	velocity.y += 8.0f;
}

/**
 * Landed after a jump.
 */
void APig::Land() {
	DebugMsg( "Landing...\n" );

	hei::Vector3 curVelocity = GetVelocity();
	if ( curVelocity.Length() > 120.0f ) {
		GetApp()->audioManager->PlayLocalSound( "audio/p_land1.wav", GetPosition(), GetVelocity(), true );
		Damage( nullptr, 20, PLVector3(), PLVector3() );
	}
}
