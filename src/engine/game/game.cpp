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

#include "../engine.h"
#include "../frontend.h"
#include "../Map.h"
#include "../language.h"
#include "../mod_support.h"

#include "actor_manager.h"
#include "mode_base.h"
#include "player.h"
#include "game.h"

#include "../script/json_reader.h"

#include "actor_pig.h"
#include "actor_static_model.h"

using namespace ohw;

std::string MapManifest::Serialize() {
	std::stringstream output;
	output << "{";
	output << R"("name":")" << name << "\",";
	output << R"("author":")" << author << "\",";
	output << R"("description":")" << description << "\",";
	if ( !modes.empty() ) {
		output << R"("modes":[)";
		for ( size_t i = 0; i < modes.size(); ++i ) {
			output << "\"" << modes[ i ] << "\"";
			if ( i != modes.size() - 1 ) {
				output << ",";
			}
		}
		output << "],";
	}
	output << R"("ambientColour":")" <<
		   std::to_string( ambient_colour.r ) << " " <<
		   std::to_string( ambient_colour.g ) << " " <<
		   std::to_string( ambient_colour.b ) << "\",";
	output << R"("skyColourTop":")" <<
		   std::to_string( sky_colour_top.r ) << " " <<
		   std::to_string( sky_colour_top.g ) << " " <<
		   std::to_string( sky_colour_top.b ) << "\",";
	output << R"("skyColourBottom":")" <<
		   std::to_string( sky_colour_bottom.r ) << " " <<
		   std::to_string( sky_colour_bottom.g ) << " " <<
		   std::to_string( sky_colour_bottom.b ) << "\",";
	output << R"("sunColour":")" <<
		   std::to_string( sun_colour.r ) << " " <<
		   std::to_string( sun_colour.g ) << " " <<
		   std::to_string( sun_colour.b ) << "\",";
	output << R"("sunYaw":")" << std::to_string( sun_yaw ) << "\",";
	output << R"("sunPitch":")" << std::to_string( sun_pitch ) << "\",";
	output << R"("temperature":")" << temperature << "\",";
	output << R"("weather":")" << weather << "\",";
	output << R"("time":")" << time << "\",";
	// Fog
	output << R"("fogColour":")" <<
		   std::to_string( fog_colour.r ) << " " <<
		   std::to_string( fog_colour.g ) << " " <<
		   std::to_string( fog_colour.b ) << "\",";
	output << R"("fogIntensity":")" << std::to_string( fog_intensity ) << "\",";
	output << R"("fogDistance":")" << std::to_string( fog_distance ) << "\"";
	output << "}\n";
	return output.str();
}

GameManager::GameManager() {
	plRegisterConsoleCommand( "CreateMap", CreateMapCommand, "Creates a new named map." );
	plRegisterConsoleCommand( "OpenMap", OpenMapCommand, "" );
	plRegisterConsoleCommand( "ListMaps", ListMapsCommand, "" );
	plRegisterConsoleCommand( "GiveItem", GiveItemCommand, "Gives a specified item to the current occupied pig." );
	plRegisterConsoleCommand( "SpawnModel", SpawnModelCommand, "Creates a model at your current position." );
	plRegisterConsoleCommand( "KillSelf", KillSelfCommand, "Kills the currently occupied pig." );

	camera_ = new Camera( { 0, 0, 0 }, { 0, 0, 0 } );
}

GameManager::~GameManager() {
	map_manifests_.clear();

	delete camera_;
}

void GameManager::Tick() {
	if ( pauseSim && simSteps == 0 ) {
		return;
	}

	FrontEnd_Tick();

	if ( mode_ == nullptr ) {
		return;
	}

	if ( ambient_emit_delay_ < g_state.sim_ticks ) {
		const AudioSample *sample = ambient_samples_[ rand() % MAX_AMBIENT_SAMPLES ];
		if ( sample != nullptr ) {
			PLVector3 position = {
				plGenerateRandomf( TERRAIN_PIXEL_WIDTH ),
				map_->GetTerrain()->GetMaxHeight(),
				plGenerateRandomf( TERRAIN_PIXEL_WIDTH )
			};
			Engine::Audio()->PlayLocalSound( sample, position, { 0, 0, 0 }, true, 0.5f );
		}

		ambient_emit_delay_ = g_state.sim_ticks + TICKS_PER_SECOND + rand() % ( 7 * TICKS_PER_SECOND );
	}

	mode_->Tick();

	ActorManager::GetInstance()->TickActors();

	if ( simSteps > 0 ) {
		simSteps--;
	}
}

void GameManager::SetupPlayers( const PlayerPtrVector &players ) {
	for ( auto i : players ) {
		GetMode()->PlayerJoined( i );
	}

	players_ = players;
}

Player *GameManager::GetPlayerByIndex( unsigned int i ) {
	if ( i >= players_.size() ) {
		LogWarn( "Invalid player index, \"%d\"!\n", i );
		return nullptr;
	}

	return players_[ i ];
}

void GameManager::LoadMap( const std::string &name ) {
	MapManifest *manifest = Engine::Game()->GetMapManifest( name );
	if ( manifest == nullptr ) {
		LogWarn( "Failed to get map descriptor, \"%s\"\n", name.c_str() );
		return;
	}

	Map *map = new Map( manifest );
	if ( map_ != nullptr ) {
		EndMode();
	}

	map_ = map;

	/* todo: we should actually pause here and wait for user input
	 *       otherwise players won't have time to read the loading screen */
	FrontEnd_SetState( FE_MODE_GAME );
}

void GameManager::UnloadMap() {
	delete map_;
}

void GameManager::CachePersistentData() {}

void GameManager::RegisterTeamManifest( const std::string &path ) {
	LogInfo( "Registering team manifest \"%s\"...\n", path.c_str() );

	JsonReader config( path );
	unsigned int numTeams = config.GetArrayLength();
	if ( numTeams == 0 ) {
		Error( "Failed to register teams, no teams available in \"%s\"!\n", path.c_str() );
	}

	for ( unsigned int i = 0; i < numTeams; ++i ) {
		config.EnterChildNode( i );

		PlayerTeam team;
		team.name = lm_gtr( config.GetStringProperty( "name", team.name ).c_str() );
		team.colour = config.GetColourProperty( "colour", PLColour( 255, 255, 255 ) );
		team.debrief_texture = config.GetStringProperty( "debriefTexture", team.debrief_texture );
		team.paper_texture = config.GetStringProperty( "paperTextures", team.paper_texture );
		team.pig_textures = config.GetStringProperty( "pigTextures", team.pig_textures );
		team.voice_set = config.GetStringProperty( "voiceSet", team.voice_set );

		defaultTeams.push_back( team );

		config.LeaveChildNode();
	}
}

void GameManager::RegisterClassManifest( const std::string &path ) {
	JsonReader config( path );

	unsigned int numClasses = config.GetArrayLength( "classes" );
	if ( numClasses == 0 ) {
		Error( "Failed to register classes, no classes available in \"%s\"!\n", path.c_str() );
	}

	config.EnterChildNode( "classes" );
	for ( unsigned int i = 0; i < numClasses; ++i ) {
		config.EnterChildNode( i );

		CharacterClass playerClass;
		playerClass.identifer = config.GetStringProperty( "identifer" );
		playerClass.cost = config.GetIntegerProperty( "cost" );
		playerClass.health = config.GetIntegerProperty( "health" );
		playerClass.model = config.GetStringProperty( "model" );
		playerClass.label = lm_gtr( config.GetStringProperty( "label" ).c_str() );

		defaultClasses.emplace( std::pair< std::string, CharacterClass >( playerClass.identifer, playerClass ) );

		config.LeaveChildNode();
	}
}

void GameManager::RegisterMapManifest( const std::string &path ) {
	LogInfo( "Registering map \"%s\"...\n", path.c_str() );

	MapManifest manifest;
	try {
		JsonReader config( path );
		manifest.name = config.GetStringProperty( "name", manifest.name );
		manifest.author = config.GetStringProperty( "author", manifest.author );
		manifest.description = config.GetStringProperty( "description", manifest.description );
		manifest.tile_directory = config.GetStringProperty( "tileDirectory", manifest.tile_directory );
		manifest.modes = config.GetArrayStrings( "modes" );
		manifest.ambient_colour = config.GetColourProperty( "ambientColour", manifest.ambient_colour );
		manifest.sky_colour_top = config.GetColourProperty( "skyColourTop", manifest.sky_colour_top );
		manifest.sky_colour_bottom = config.GetColourProperty( "skyColourBottom", manifest.sky_colour_bottom );
		manifest.sun_colour = config.GetColourProperty( "sunColour", manifest.sun_colour );
		manifest.sun_yaw = config.GetFloatProperty( "sunYaw", manifest.sun_yaw );
		manifest.sun_pitch = config.GetFloatProperty( "sunPitch", manifest.sun_pitch );
		manifest.temperature = config.GetStringProperty( "temperature", manifest.temperature );
		manifest.weather = config.GetStringProperty( "weather", manifest.weather );
		manifest.time = config.GetStringProperty( "time", manifest.time );

		// Fog
		manifest.fog_colour = config.GetColourProperty( "fogColour", manifest.fog_colour );
		manifest.fog_intensity = config.GetFloatProperty( "fogIntensity", manifest.fog_intensity );
		manifest.fog_distance = config.GetFloatProperty( "fogDistance", manifest.fog_distance );
	} catch ( const std::exception &e ) {
		LogWarn( "Failed to read map config, \"%s\"!\n%s\n", path.c_str(), e.what() );
	}

	char temp_buf[64];
	plStripExtension( temp_buf, sizeof( temp_buf ), plGetFileName( path.c_str() ) );

	manifest.filepath = path;
	manifest.filename = temp_buf;

	map_manifests_.insert( std::make_pair( temp_buf, manifest ) );
}

static void RegisterManifestInterface( const char *path, void *userData ) {
	u_unused( userData );
	Engine::Game()->RegisterMapManifest( path );
}

/**
 * Scans the campaigns directory for .map files and indexes them.
 */
void GameManager::RegisterMapManifests() {
	map_manifests_.clear();
	plScanDirectory( "maps", "map", RegisterManifestInterface, false, nullptr );
}

/**
 * Returns a pointer to the requested manifest.
 * @param name
 * @return Returns a pointer to the requested manifest, otherwise returns null.
 */
MapManifest *GameManager::GetMapManifest( const std::string &name ) {
	auto manifest = map_manifests_.find( name );
	if ( manifest != map_manifests_.end() ) {
		return &manifest->second;
	}

	LogWarn( "Failed to get manifest for \"%s\"!\n", name.c_str() );
	return nullptr;
}

/**
 * Creates a new map manifest and writes it into the maps directory.
 * @param name
 * @return
 */
MapManifest *GameManager::CreateManifest( const std::string &name ) {
	// ensure the map doesn't exist already
	if ( Engine::Game()->GetMapManifest( name ) != nullptr ) {
		LogWarn( "Unable to create map, it already exists!\n" );
		return nullptr;
	}

	const ModDirectory *currentMod = Mod_GetCurrentMod();
	std::string path = "mods/" + currentMod->directory + "maps/" + name + ".map";
	std::ofstream output( path );
	if ( !output.is_open() ) {
		LogWarn( "Failed to write to \"%s\", aborting!n\"\n", path.c_str() );
		return nullptr;
	}

	MapManifest manifest;
	output << manifest.Serialize();
	output.close();

	LogInfo( "Wrote \"%s\"!\n", path.c_str() );

	Engine::Game()->RegisterMapManifest( path );
	return Engine::Game()->GetMapManifest( name );
}

const CharacterClass *GameManager::GetDefaultClass( const std::string &classIdentifer ) const {
	const auto &i = defaultClasses.find( classIdentifer );
	if( i == defaultClasses.end() ) {
		LogWarn( "Failed to find character class \"%s\"!\n", classIdentifer.c_str() );
		return nullptr;
	}

	return &i->second;
}

void GameManager::CreateMapCommand( unsigned int argc, char **argv ) {
	if ( argc < 2 ) {
		LogWarn( "Invalid number of arguments, ignoring!\n" );
		return;
	}

	MapManifest *manifest = Engine::Game()->CreateManifest( argv[ 1 ] );
	if ( manifest == nullptr ) {
		return;
	}

	Engine::Game()->LoadMap( argv[ 1 ] );
}

/**
 * Loads the specified map.
 * @param argc
 * @param argv
 */
void GameManager::OpenMapCommand( unsigned int argc, char **argv ) {
	if ( argc < 2 ) {
		LogWarn( "Invalid number of arguments, ignoring!\n" );
		return;
	}

	std::string mode = "singleplayer";
	const MapManifest *desc = Engine::Game()->GetMapManifest( argv[ 1 ] );
	if ( desc != nullptr && !desc->modes.empty() ) {
		mode = desc->modes[ 0 ];
	}

	// Set up a mode with some defaults.
	GameModeDescriptor descriptor = GameModeDescriptor();
	Engine::Game()->StartMode( argv[ 1 ], { new Player( PlayerType::LOCAL ) }, descriptor );
}

/**
 * Provide a list of all the currently registered maps.
 * @param argc
 * @param argv
 */
void GameManager::ListMapsCommand( unsigned int argc, char **argv ) {
	if ( Engine::Game()->map_manifests_.empty() ) {
		LogWarn( "No maps available!\n" );
		return;
	}

	for ( auto manifest : Engine::Game()->map_manifests_ ) {
		MapManifest *desc = &manifest.second;
		std::string out =
			desc->name + "/" + manifest.first +
				" : " + desc->description +
				" :";
		// print out all the supported modes
		for ( size_t i = 0; i < desc->modes.size(); ++i ) {
			out += ( i == 0 ? " " : ", " ) + desc->modes[ i ];
		}
		LogInfo( "%s\n", out.c_str() );
	}

	LogInfo( "%u maps\n", Engine::Game()->map_manifests_.size() );
}

void GameManager::GiveItemCommand( unsigned int argc, char **argv ) {
	if ( argc < 2 ) {
		LogWarn( "Invalid number of arguments, ignoring!\n" );
		return;
	}

	Player *player = Engine::Game()->mode_->GetCurrentPlayer();
	if ( player == nullptr ) {
		LogWarn( "Failed to get current player!\n" );
		return;
	}

	Actor *actor = player->GetCurrentChild();
	if ( actor == nullptr ) {
		LogWarn( "No actor currently active!\n" );
		return;
	}

	APig *pig = dynamic_cast<APig *>(actor);
	if ( pig == nullptr ) {
		LogWarn( "Actor is not a pig!\n" );
		return;
	}

	auto item = static_cast<ItemIdentifier>(strtol( argv[ 1 ], nullptr, 10 ));
	unsigned int quantity = 1;
	if ( argc > 2 ) {
		quantity = strtol( argv[ 2 ], nullptr, 10 );
	}

	pig->AddInventoryItem( item, quantity );
}

void GameManager::KillSelfCommand( unsigned int argc, char **argv ) {
	if ( Engine::Game()->mode_ == nullptr ) {
		LogWarn( "Command cannot function outside of game!\n" );
		return;
	}

	Player *player = Engine::Game()->mode_->GetCurrentPlayer();
	if ( player == nullptr ) {
		LogWarn( "Failed to get current player!\n" );
		return;
	}

	Actor *actor = player->GetCurrentChild();
	if ( actor == nullptr ) {
		LogWarn( "No actor currently active!\n" );
		return;
	}

	actor->Damage( nullptr, 9999 );
}

void GameManager::SpawnModelCommand( unsigned int argc, char **argv ) {
	if ( Engine::Game()->mode_ == nullptr ) {
		LogInfo( "Command cannot function outside of game!\n" );
		return;
	} else if ( argc < 2 ) {
		LogWarn( "Invalid number of arguments, ignoring!\n" );
		return;
	}

	Player *player = Engine::Game()->mode_->GetCurrentPlayer();
	if ( player == nullptr ) {
		LogWarn( "Failed to get current player!\n" );
		return;
	}

	// Fetch the player's actor, so we can get their position
	Actor *actor = player->GetCurrentChild();
	if ( actor == nullptr ) {
		LogWarn( "No actor currently active!\n" );
		return;
	}

	AStaticModel
		*model_actor = dynamic_cast<AStaticModel *>(ActorManager::GetInstance()->CreateActor( "AStaticModel" ));
	if ( model_actor == nullptr ) {
		Error( "Failed to create model actor!\n" );
	}

	model_actor->SetModel( argv[ 1 ] );
	model_actor->SetPosition( actor->GetPosition() );
	model_actor->SetAngles( actor->GetAngles() );
}

void GameManager::StartMode( const std::string &map,
							 const PlayerPtrVector &players,
							 const GameModeDescriptor &descriptor ) {
	FrontEnd_SetState( FE_MODE_LOADING );

	// Free up all our unreferenced resources
	Engine::Resource()->ClearAllResources();

	LoadMap( map );

	if ( map_ == nullptr ) {
		LogWarn( "Failed to start mode, map wasn't loaded!\n" );
		EndMode();
		return;
	}

	std::string sample_ext = "d";
	if ( map_->GetManifest()->time != "day" ) {
		sample_ext = "n";
	}

	ambient_emit_delay_ = g_state.sim_ticks + plGenerateRandomd( 100 ) + 1;
	for ( unsigned int i = 1, idx = 0; i < 4; ++i ) {
		std::string snum = std::to_string( i );
		std::string path = "audio/amb_";
		if ( i < 3 ) {
			path += snum + sample_ext + ".wav";
			ambient_samples_[ idx++ ] = Engine::Audio()->CacheSample( path, false );
		}

		path = "audio/batt_s" + snum + ".wav";
		ambient_samples_[ idx++ ] = Engine::Audio()->CacheSample( path, false );
		path = "audio/batt_l" + snum + ".wav";
		ambient_samples_[ idx++ ] = Engine::Audio()->CacheSample( path, false );
	}

	FrontEnd_SetState( FE_MODE_GAME );

	// call StartRound; deals with spawning everything in and other mode specific logic
	mode_ = new BaseGameMode( descriptor );

	SetupPlayers( players );

	mode_->StartRound();
}

/**
 * End the currently active mode and flush everything.
 */
void GameManager::EndMode() {
	delete mode_;

	// Clear out all the allocated players for this game
	for ( auto i : players_ ) {
		delete i;
	}
	players_.clear();

	UnloadMap();

	ActorManager::GetInstance()->DestroyActors();

	// Free up all our unreferenced resources
	Engine::Resource()->ClearAllResources();

	Engine::Audio()->FreeSources();
	Engine::Audio()->FreeSamples();
}

/**
 * Check whether or not a mode is currently active.
 * @return Returns true if a mode is currently active.
 */
bool GameManager::IsModeActive() {
	return ( mode_ != nullptr );
}
