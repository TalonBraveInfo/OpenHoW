/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

#include "App.h"
#include "Map.h"
#include "GameMode.h"
#include "ActorManager.h"
#include "Player.h"
#include "APig.h"
#include "AAirship.h"
#include "graphics/Camera.h"

using namespace ohw;

GameMode::GameMode( const GameModeDescriptor &descriptor ) {
	max_turn_ticks = descriptor.turn_time * TICKS_PER_SECOND;
}

GameMode::~GameMode() = default;

void GameMode::StartRound() {
	if ( HasRoundStarted() ) {
		Error( "Attempted to change map in the middle of a round, aborting!\n" );
	}

	SpawnActors();

	// Play the deployment music
	GetApp()->audioManager->PlayMusic( "music/track" + std::to_string( std::rand() % 4 + 27 ) + ".ogg" );

	StartTurn( GetCurrentPlayer() );

	hasRoundStarted = true;
}

void GameMode::RestartRound() {
	DestroyActors();

	StartRound();
}

void GameMode::EndRound() {
	DestroyActors();
}

void GameMode::Tick() {
	if ( !HasRoundStarted() ) {
		// still setting the game up...
		return;
	}

	Player *player = GetCurrentPlayer();
	if ( player == nullptr ) {
		return;
	}

	if ( !HasTurnStarted() ) {
		StartTurn( player );
		return;
	}

	Actor *actor = player->GetCurrentChild();
	if ( actor == nullptr ) {
		return;
	}

	actor->HandleInput();

	if ( HasTurnStarted() ) {
		num_turn_ticks++;
		if ( num_turn_ticks >= max_turn_ticks ) {
			EndTurn( player );
		}
	}
}

void GameMode::SpawnActors() {
	Map *map = GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		Error( "Attempted to spawn actors without having loaded a map!\n" );
		return;
	}

	const std::vector< ActorSpawn > &spawns = map->GetSpawns();
	for ( const auto &spawn : spawns ) {
		Actor *actor = ActorManager::GetInstance()->CreateActor( spawn.className, spawn );
		if ( actor == nullptr ) {
			actor = ActorManager::GetInstance()->CreateActor( "model_static", spawn );
		}

		APig *pig = dynamic_cast<APig *>(actor);
		if ( pig == nullptr ) {
			continue;
		}

		Player *player = GetApp()->gameManager->GetPlayerByIndex( pig->GetTeam() );
		if ( player == nullptr ) {
			Warning( "Failed to assign pig to team!\n" );
			continue;
		}

		pig->SetPlayerOwner( player );
	}

	AAirship *model_actor = dynamic_cast<AAirship *>(ActorManager::GetInstance()->CreateActor( "vehicle_airship" ));
	if ( model_actor == nullptr ) {
		Error( "Failed to create model actor!\n" );
	}

	ActorManager::GetInstance()->ActivateActors();
}

void GameMode::DestroyActors() {
	ActorManager::GetInstance()->DestroyActors();
}

void GameMode::StartTurn( Player *player ) {
	if ( player->GetNumChildren() == 0 ) {
		Warning( "No valid control target for player \"%s\"!\n", player->GetTeam()->name.c_str() );
		return;
	}

	player->PossessCurrentChild();

	turn_started_ = true;
}

void GameMode::EndTurn( Player *player ) {
	player->DispossessCurrentChild();
	player->CycleChildren();

	// move onto the next player
	CyclePlayers();

	num_turn_ticks = 0;
	turn_started_ = false;
}

void GameMode::PlayerJoined( Player *player ) {
	// todo: display prompt
	Print( "%s has joined the game\n", player->GetTeam()->name.c_str() );
}

void GameMode::PlayerLeft( Player *player ) {
	// todo: display prompt
	Print( "%s has left the game\n", player->GetTeam()->name.c_str() );
}

unsigned int GameMode::GetMaxSpectators() const {
	return 0;
}

void GameMode::SpectatorJoined( Player *player ) {
	// todo: display prompt
	Print( "%s has joined the spectators\n", player->GetTeam()->name.c_str() );
}

void GameMode::SpectatorLeft( Player *player ) {
	// todo: display prompt
	Print( "%s has left the spectators\n", player->GetTeam()->name.c_str() );
}

/**
 * Maximum players allowed in this mode.
 * @return Number of players.
 */
unsigned int GameMode::GetMaxPlayers() const {
	return 4;
}

Player *GameMode::GetCurrentPlayer() {
	return GetApp()->gameManager->GetPlayerByIndex( currentPlayer );
}

Actor *GameMode::GetPossessedActor() {
	Player *player = GetCurrentPlayer();
	if ( player == nullptr ) {
		return nullptr;
	}

	return player->GetCurrentChild();
}

void GameMode::CyclePlayers() {
	currentPlayer++;

	PlayerPtrVector players = GetApp()->gameManager->GetPlayers();
	if ( currentPlayer >= players.size() ) {
		currentPlayer = 0;
	}
}

void GameMode::AssignActorToPlayer( Actor *target, Player *owner ) {
	owner->AddChild( target );
}
