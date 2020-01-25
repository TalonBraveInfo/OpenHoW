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

#include "../engine.h"
#include "../Map.h"

#include "mode_base.h"
#include "actor_manager.h"
#include "player.h"
#include "actors/actor_pig.h"
#include "actors/actor_static_model.h"

using namespace openhow;

BaseGameMode::BaseGameMode( const GameModeDescriptor& descriptor ) {
	max_turn_ticks = descriptor.turn_time * TICKS_PER_SECOND;
}

BaseGameMode::~BaseGameMode() = default;

void BaseGameMode::StartRound() {
	if ( HasRoundStarted() ) {
		Error( "Attempted to change map in the middle of a round, aborting!\n" );
	}

	SpawnActors();

	// Play the deployment music
	Engine::Audio()->PlayMusic( "music/track" + std::to_string( std::rand() % 4 + 27 ) + ".ogg" );

	StartTurn( GetCurrentPlayer() );

	round_started_ = true;
}

void BaseGameMode::RestartRound() {
	DestroyActors();

	StartRound();
}

void BaseGameMode::EndRound() {
	DestroyActors();
}

void BaseGameMode::Tick() {
	if ( !HasRoundStarted() ) {
		// still setting the game up...
		return;
	}

	Player* player = GetCurrentPlayer();
	if ( player == nullptr ) {
		return;
	}

	Actor* actor = player->GetCurrentChild();
	if ( actor == nullptr ) {
		return;
	}

	actor->HandleInput();

	// temp: force the camera at the actor pos

	PLVector3 forward = actor->GetForward();
	Camera* camera = Engine::Game()->GetCamera();
	camera->SetPosition( {
							 actor->GetPosition().x + forward.x * -500,
							 actor->GetPosition().y + 500.f,
							 actor->GetPosition().z + forward.z * -500 } );
	camera->SetAngles( { -25.f, actor->GetAngles().y, 0 } );

	if ( HasTurnStarted() ) {
		num_turn_ticks++;
		if ( num_turn_ticks >= max_turn_ticks ) {

		}
	}
}

void BaseGameMode::SpawnActors() {
	Map* map = Engine::Game()->GetCurrentMap();
	if ( map == nullptr ) {
		Error( "Attempted to spawn actors without having loaded a map!\n" );
	}

	std::vector<ActorSpawn> spawns = map->GetSpawns();
	for ( const auto& spawn : spawns ) {
		Actor* actor = ActorManager::GetInstance()->CreateActor( spawn.class_name );
		if ( actor == nullptr ) {
			actor = ActorManager::GetInstance()->CreateActor( "static_model" );
		}

		actor->Deserialize( spawn );

		APig* pig = dynamic_cast<APig*>(actor);
		if ( pig == nullptr ) {
			continue;
		}

		Player* player = Engine::Game()->GetPlayerByIndex( pig->GetTeam() );
		if ( player == nullptr ) {
			LogWarn( "Failed to assign pig to team!\n" );
			continue;
		}

		AssignActorToPlayer( pig, player );
	}

	// TEMP START

	AStaticModel* model_actor = dynamic_cast<AStaticModel*>(ActorManager::GetInstance()->CreateActor( "static_model" ));
	if ( model_actor == nullptr ) {
		Error( "Failed to create model actor!\n" );
	}

	model_actor->SetModel( "scenery/airship1" );
	model_actor->SetPosition( {
								  TERRAIN_PIXEL_WIDTH / 2, TERRAIN_PIXEL_WIDTH / 2,
								  Engine::Game()->GetCurrentMap()->GetTerrain()->GetMaxHeight() } );

	// TEMP END

	ActorManager::GetInstance()->ActivateActors();
}

void BaseGameMode::DestroyActors() {
	ActorManager::GetInstance()->DestroyActors();
}

void BaseGameMode::StartTurn( Player* player ) {
	if ( player->GetNumChildren() == 0 ) {
		LogWarn( "No valid control target for player \"%s\"!\n", player->GetTeam()->name.c_str() );
		return;
	}

	player->PossessCurrentChild();

	turn_started_ = true;
}

void BaseGameMode::EndTurn( Player* player ) {
	player->DepossessCurrentChild();
	player->CycleChildren();

	// move onto the next player
	CyclePlayers();

	num_turn_ticks = 0;

	turn_started_ = false;
}

void BaseGameMode::PlayerJoined( Player* player ) {
	// todo: display prompt
	LogInfo( "%s has joined the game\n", player->GetTeam()->name.c_str() );
}

void BaseGameMode::PlayerLeft( Player* player ) {
	// todo: display prompt
	LogInfo( "%s has left the game\n", player->GetTeam()->name.c_str() );
}

unsigned int BaseGameMode::GetMaxSpectators() const {
	return 0;
}

void BaseGameMode::SpectatorJoined( Player* player ) {
	// todo: display prompt
	LogInfo( "%s has joined the spectators\n", player->GetTeam()->name.c_str() );
}

void BaseGameMode::SpectatorLeft( Player* player ) {
	// todo: display prompt
	LogInfo( "%s has left the spectators\n", player->GetTeam()->name.c_str() );
}

/**
 * Maximum players allowed in this mode.
 * @return Number of players.
 */
unsigned int BaseGameMode::GetMaxPlayers() const {
	return 4;
}

Player* BaseGameMode::GetCurrentPlayer() {
	return Engine::Game()->GetPlayerByIndex( current_player_ );
}

void BaseGameMode::CyclePlayers() {
	PlayerPtrVector players = Engine::Game()->GetPlayers();
	if ( current_player_ >= players.size() ) {
		current_player_ = 0;
	} else {
		current_player_++;
	}
}

void BaseGameMode::AssignActorToPlayer( Actor* target, Player* owner ) {
	owner->AddChild( target );
}
