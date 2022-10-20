// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include <fstream>

#include "App.h"
#include "Menu.h"
#include "Map.h"
#include "Language.h"
#include "ActorManager.h"
#include "GameMode.h"
#include "Player.h"

#include "graphics/Camera.h"

#include "script/JsonReader.h"

#include "APig.h"
#include "AStaticModel.h"
#include "GameManager.h"

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

ohw::GameManager::GameManager() {
	PlRegisterConsoleCommand( "CreateMap", "Creates a new named map.", 1, CreateMapCommand );
	PlRegisterConsoleCommand( "OpenMap", "Opens the specified map.", 1, OpenMapCommand );
	PlRegisterConsoleCommand( "ListMaps", "Lists all of the existing maps.", 0, ListMapsCommand );
	PlRegisterConsoleCommand( "GiveItem", "Gives a specified item to the current occupied actor.", 1, GiveItemCommand );
	PlRegisterConsoleCommand( "SpawnModel", "Creates a model at your current position.", 1, SpawnModelCommand );
	PlRegisterConsoleCommand( "KillSelf", "Kills the currently occupied actor.", 1, KillSelfCommand );
	PlRegisterConsoleCommand( "Teleport", "Teleports current actor to the given destination.", 1, TeleportCommand );
	PlRegisterConsoleCommand( "FirstPerson", "Toggles the camera into first-person mode.", 1, FirstPersonCommand );
	PlRegisterConsoleCommand( "FreeCam", "Toggles the camera into fly mode.", 1, FreeCamCommand );

	defaultCamera = new Camera( pl_vecOrigin3, pl_vecOrigin3 );
}

ohw::GameManager::~GameManager() {
	mapManifests.clear();

	delete defaultCamera;
}

void ohw::GameManager::Tick() {
	if ( pauseSim && simSteps == 0 ) {
		return;
	}

	FrontEnd_Tick();

	if ( currentMode == nullptr ) {
		return;
	}

	if ( ambient_emit_delay_ < GetApp()->GetSimulationTicks() ) {
		const AudioSample *sample = ambient_samples_[ rand() % MAX_AMBIENT_SAMPLES ];
		if ( sample != nullptr ) {
			PLVector3 position = {
					PlGenerateRandomFloat( TERRAIN_PIXEL_WIDTH ),
					currentMap->GetTerrain()->GetMaxHeight(),
					PlGenerateRandomFloat( TERRAIN_PIXEL_WIDTH )
			};
			GetApp()->audioManager->PlayLocalSound( sample, position, { 0, 0, 0 }, true, 0.5f );
		}

		ambient_emit_delay_ = GetApp()->GetSimulationTicks() + TICKS_PER_SECOND + rand() % ( 7 * TICKS_PER_SECOND );
	}

	currentMode->Tick();

	TickCamera();

	ActorManager::GetInstance()->TickActors();

	if ( simSteps > 0 ) {
		simSteps--;
	}
}

void ohw::GameManager::SetupPlayers( const PlayerPtrVector &players ) {
	for ( auto i: players ) {
		GetMode()->PlayerJoined( i );
	}

	players_ = players;
}

void ohw::GameManager::ClearPlayers() {
	for ( auto i: players_ ) {
		delete i;
	}
	players_.clear();
}

Player *ohw::GameManager::GetPlayerByIndex( unsigned int i ) const {
	if ( i >= players_.size() ) {
		Warning( "Invalid player index, \"%d\"!\n", i );
		return nullptr;
	}

	return players_[ i ];
}

void ohw::GameManager::LoadMap( const std::string &name ) {
	MapManifest *manifest = GetApp()->gameManager->GetMapManifest( name );
	if ( manifest == nullptr ) {
		Warning( "Failed to get map descriptor, \"%s\"\n", name.c_str() );
		return;
	}

	Map *map = new Map( manifest );
	if ( currentMap != nullptr ) {
		EndMode();
	}

	currentMap = map;

	/* todo: we should actually pause here and wait for user input
	 *       otherwise players won't have time to read the loading screen */
	FrontEnd_SetState( FE_MODE_GAME );
}

void ohw::GameManager::UnloadMap() {
	delete currentMap;
}

void ohw::GameManager::CachePersistentData() {
	RegisterMapManifests();
	RegisterTeamManifest( "scripts/teams.json" );
	RegisterClassManifest( "scripts/classes.json" );

	// todo: ActorManager should be part of the GameManager class
	ActorManager::GetInstance()->RegisterSpawnManifests();
}

void ohw::GameManager::RegisterTeamManifest( const std::string &path ) {
	Print( "Registering team manifest \"%s\"...\n", path.c_str() );

	JsonReader config( path );
	unsigned int numTeams = config.GetArrayLength();
	if ( numTeams == 0 ) {
		Error( "Failed to register teams, no teams available in \"%s\"!\n", path.c_str() );
	}

	for ( unsigned int i = 0; i < numTeams; ++i ) {
		config.EnterChildNode( i );

		PlayerTeam team;
		team.name = lm_gtr( config.GetStringProperty( "name", team.name, false ).c_str() );
		team.colour = config.GetColourProperty( "colour", hei::Colour( 255, 255, 255 ), false );
		team.debrief_texture = config.GetStringProperty( "debriefTexture", team.debrief_texture, false );
		team.paper_texture = config.GetStringProperty( "paperTextures", team.paper_texture, false );
		team.pig_textures = config.GetStringProperty( "pigTextures", team.pig_textures, false );
		team.voice_set = config.GetStringProperty( "voiceSet", team.voice_set, false );

		defaultTeams.push_back( team );

		config.LeaveChildNode();
	}
}

void ohw::GameManager::RegisterClassManifest( const std::string &path ) {
	JsonReader config( path );

	unsigned int numClasses = config.GetArrayLength( "classes" );
	if ( numClasses == 0 ) {
		Error( "Failed to register classes, no classes available in \"%s\"!\n", path.c_str() );
	}

	config.EnterChildNode( "classes" );
	for ( unsigned int i = 0; i < numClasses; ++i ) {
		config.EnterChildNode( i );

		CharacterClass playerClass;
		playerClass.identifer = config.GetStringProperty( "identifier" );
		playerClass.cost = config.GetIntegerProperty( "cost" );
		playerClass.health = config.GetIntegerProperty( "health" );
		playerClass.model = config.GetStringProperty( "model" );
		playerClass.label = lm_gtr( config.GetStringProperty( "label" ).c_str() );

		defaultClasses.emplace( std::pair< std::string, CharacterClass >( playerClass.identifer, playerClass ) );

		config.LeaveChildNode();
	}
}

void ohw::GameManager::RegisterMapManifest( const std::string &path ) {
	Print( "Registering map \"%s\"...\n", path.c_str() );

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
		Warning( "Failed to read map config, \"%s\"!\n%s\n", path.c_str(), e.what() );
	}

	char temp_buf[64];
	PlStripExtension( temp_buf, sizeof( temp_buf ), PlGetFileName( path.c_str() ) );

	manifest.filepath = path;
	manifest.filename = temp_buf;

	mapManifests.insert( std::make_pair( temp_buf, manifest ) );
}

static void RegisterManifestInterface( const char *path, void *userPtr ) {
	u_unused( userPtr );
	ohw::GetApp()->gameManager->RegisterMapManifest( path );
}

/**
 * Scans the campaigns directory for .map files and indexes them.
 */
void ohw::GameManager::RegisterMapManifests() {
	mapManifests.clear();
	PlScanDirectory( "maps", "map", RegisterManifestInterface, false, nullptr );
}

/**
 * Returns a pointer to the requested manifest.
 * @param name
 * @return Returns a pointer to the requested manifest, otherwise returns null.
 */
MapManifest *ohw::GameManager::GetMapManifest( const std::string &name ) {
	auto manifest = mapManifests.find( name );
	if ( manifest != mapManifests.end() ) {
		return &manifest->second;
	}

	Warning( "Failed to get manifest for \"%s\"!\n", name.c_str() );
	return nullptr;
}

/**
 * Creates a new map manifest and writes it into the maps directory.
 * @param name
 * @return
 */
MapManifest *ohw::GameManager::CreateManifest( const std::string &name ) {
	// ensure the map doesn't exist already
	if ( GetApp()->gameManager->GetMapManifest( name ) != nullptr ) {
		Warning( "Unable to create map, it already exists!\n" );
		return nullptr;
	}

	const ModManager::ModDescription *currentMod = GetApp()->modManager->GetCurrentModDescription();
	std::string path = "mods/" + currentMod->directory + "maps/" + name + ".map";
	std::ofstream output( path );
	if ( !output.is_open() ) {
		Warning( "Failed to write to \"%s\", aborting!n\"\n", path.c_str() );
		return nullptr;
	}

	MapManifest manifest;
	output << manifest.Serialize();
	output.close();

	Print( "Wrote \"%s\"!\n", path.c_str() );

	GetApp()->gameManager->RegisterMapManifest( path );
	return GetApp()->gameManager->GetMapManifest( name );
}

const CharacterClass *ohw::GameManager::GetDefaultClass( const std::string &classIdentifer ) const {
	const auto &i = defaultClasses.find( classIdentifer );
	if ( i == defaultClasses.end() ) {
		Warning( "Failed to find character class \"%s\"!\n", classIdentifer.c_str() );
		return nullptr;
	}

	return &i->second;
}

void ohw::GameManager::CreateMapCommand( unsigned int argc, char **argv ) {
	if ( argc < 2 ) {
		Warning( "Invalid number of arguments, ignoring!\n" );
		return;
	}

	MapManifest *manifest = GetApp()->gameManager->CreateManifest( argv[ 1 ] );
	if ( manifest == nullptr ) {
		return;
	}

	GetApp()->gameManager->LoadMap( argv[ 1 ] );
}

/**
 * Loads the specified map.
 */
void ohw::GameManager::OpenMapCommand( unsigned int argc, char **argv ) {
	if ( argc < 2 ) {
		Warning( "Invalid number of arguments, ignoring!\n" );
		return;
	}

	std::string modeString = "singleplayer";
	const MapManifest *desc = GetApp()->gameManager->GetMapManifest( argv[ 1 ] );
	if ( desc != nullptr && !desc->modes.empty() ) {
		modeString = desc->modes[ 0 ];
	}

	// Set up a mode with some defaults.
	GameModeDescriptor descriptor = GameModeDescriptor();
	GetApp()->gameManager->StartMode( argv[ 1 ], { new Player( PlayerType::LOCAL ) }, descriptor );
}

/**
 * Provide a list of all the currently registered maps.
 */
void ohw::GameManager::ListMapsCommand( unsigned int argc, char **argv ) {
	if ( GetApp()->gameManager->mapManifests.empty() ) {
		Warning( "No maps available!\n" );
		return;
	}

	for ( auto manifest: GetApp()->gameManager->mapManifests ) {
		MapManifest *desc = &manifest.second;
		std::string out =
				desc->name + "/" + manifest.first +
				" : " + desc->description +
				" :";
		// print out all the supported modes
		for ( size_t i = 0; i < desc->modes.size(); ++i ) {
			out += ( i == 0 ? " " : ", " ) + desc->modes[ i ];
		}
		Print( "%s\n", out.c_str() );
	}

	Print( "%u maps\n", GetApp()->gameManager->mapManifests.size() );
}

void ohw::GameManager::GiveItemCommand( unsigned int argc, char **argv ) {
	if ( argc < 2 ) {
		Warning( "Invalid number of arguments, ignoring!\n" );
		return;
	}

	GameMode *mode = dynamic_cast<GameMode *>(GetApp()->gameManager->GetMode());
	if ( mode == nullptr ) {
		Print( "Command cannot function outside of game!\n" );
		return;
	}

	Actor *actor = mode->GetPossessedActor();
	if ( actor == nullptr ) {
		Warning( "No actor currently active!\n" );
		return;
	}

	APig *pig = dynamic_cast<APig *>(actor);
	if ( pig == nullptr ) {
		Warning( "Actor is not a pig!\n" );
		return;
	}

	auto item = static_cast<ItemIdentifier>(strtol( argv[ 1 ], nullptr, 10 ));
	unsigned int quantity = 1;
	if ( argc > 2 ) {
		quantity = strtol( argv[ 2 ], nullptr, 10 );
	}

	pig->AddInventoryItem( item, quantity );
}

void ohw::GameManager::KillSelfCommand( unsigned int argc, char **argv ) {
	GameMode *mode = dynamic_cast<GameMode *>(GetApp()->gameManager->GetMode());
	if ( mode == nullptr ) {
		Print( "Command cannot function outside of game!\n" );
		return;
	}

	Actor *actor = mode->GetPossessedActor();
	if ( actor == nullptr ) {
		Warning( "No actor currently active!\n" );
		return;
	}

	actor->Damage( nullptr, 9999 );
}

void ohw::GameManager::SpawnModelCommand( unsigned int argc, char **argv ) {
	if ( argc < 2 ) {
		Warning( "Invalid number of arguments, ignoring!\n" );
		return;
	}

	GameMode *mode = dynamic_cast<GameMode *>(GetApp()->gameManager->GetMode());
	if ( mode == nullptr ) {
		Print( "Command cannot function outside of game!\n" );
		return;
	}

	// Fetch the player's actor, so we can get their position
	Actor *actor = mode->GetPossessedActor();
	if ( actor == nullptr ) {
		Warning( "No actor currently active!\n" );
		return;
	}

	AStaticModel *modelActor = dynamic_cast<AStaticModel *>(ActorManager::GetInstance()->CreateActor( "model_static" ));
	if ( modelActor == nullptr ) {
		Error( "Failed to create model actor!\n" );
		return;
	}

	modelActor->SetModel( argv[ 1 ] );
	modelActor->SetPosition( actor->GetPosition() );
	modelActor->SetAngles( actor->GetAngles() );
}

void ohw::GameManager::TeleportCommand( unsigned int argc, char **argv ) {
	if ( argc < 4 ) {
		Warning( "Invalid number of arguments, ignoring!\n" );
		return;
	}

	GameMode *mode = dynamic_cast<GameMode *>(GetApp()->gameManager->GetMode());
	if ( mode == nullptr ) {
		Print( "Command cannot function outside of game!\n" );
		return;
	}

	// Fetch the player's actor, so we can get their position
	Actor *actor = mode->GetPossessedActor();
	if ( actor == nullptr ) {
		Warning( "No actor currently active!\n" );
		return;
	}

	hei::Vector3 teleportDestination(
			strtof( argv[ 1 ], nullptr ),
			strtof( argv[ 2 ], nullptr ),
			strtof( argv[ 3 ], nullptr ) );
	actor->SetPosition( teleportDestination );
}

void ohw::GameManager::FirstPersonCommand( unsigned int argc, char **argv ) {
	static CameraMode oldCameraMode = CameraMode::FOLLOW;

	if ( GetApp()->gameManager->cameraMode != CameraMode::FIRSTPERSON ) {
		oldCameraMode = GetApp()->gameManager->cameraMode;
		GetApp()->gameManager->cameraMode = CameraMode::FIRSTPERSON;
		return;
	}

	GetApp()->gameManager->cameraMode = oldCameraMode;
}

void ohw::GameManager::FreeCamCommand( unsigned int argc, char **argv ) {
	static CameraMode oldCameraMode = CameraMode::FOLLOW;

	if ( GetApp()->gameManager->cameraMode != CameraMode::FLY ) {
		oldCameraMode = GetApp()->gameManager->cameraMode;
		GetApp()->gameManager->cameraMode = CameraMode::FLY;
		return;
	}

	GetApp()->gameManager->cameraMode = oldCameraMode;
}

void ohw::GameManager::StartMode( const std::string &map,
                                  const PlayerPtrVector &players,
                                  const GameModeDescriptor &descriptor ) {
	FrontEnd_SetState( FE_MODE_LOADING );

	// Free up all our unreferenced resources
	GetApp()->resourceManager->ClearAllResources();

	LoadMap( map );

	if ( currentMap == nullptr ) {
		Warning( "Failed to start mode, map wasn't loaded!\n" );
		EndMode();
		return;
	}

	std::string sample_ext = "d";
	if ( currentMap->GetManifest()->time != "day" ) {
		sample_ext = "n";
	}

	ambient_emit_delay_ = GetApp()->GetSimulationTicks() + PlGenerateRandomDouble( 100 ) + 1;
	for ( unsigned int i = 1, idx = 0; i < 4; ++i ) {
		std::string snum = std::to_string( i );
		std::string path = "audio/amb_";
		if ( i < 3 ) {
			path += snum + sample_ext + ".wav";
			ambient_samples_[ idx++ ] = GetApp()->audioManager->CacheSample( path, false );
		}

		path = "audio/batt_s" + snum + ".wav";
		ambient_samples_[ idx++ ] = GetApp()->audioManager->CacheSample( path, false );
		path = "audio/batt_l" + snum + ".wav";
		ambient_samples_[ idx++ ] = GetApp()->audioManager->CacheSample( path, false );
	}

	FrontEnd_SetState( FE_MODE_GAME );

	// call StartRound; deals with spawning everything in and other mode specific logic
	currentMode = new GameMode( descriptor );

	SetupPlayers( players );

	currentMode->StartRound();
}

/**
 * End the currently active mode and flush everything.
 */
void ohw::GameManager::EndMode() {
	delete currentMode;

	// Clear out all the allocated players for this game
	ClearPlayers();

	UnloadMap();

	ActorManager::GetInstance()->DestroyActors();

	// Free up all our unreferenced resources
	GetApp()->resourceManager->ClearAllResources();

	GetApp()->audioManager->FreeSources();
	GetApp()->audioManager->FreeSamples();
}

/**
 * Check whether or not a mode is currently active.
 * @return Returns true if a mode is currently active.
 */
bool ohw::GameManager::IsModeActive() {
	return ( currentMode != nullptr );
}

/**
 * Handle the different camera modes.
 */
void ohw::GameManager::TickCamera() {
	switch ( cameraMode ) {
		case CameraMode::FLY:
			break;
		case CameraMode::FIRSTPERSON: {
			GameMode *mode = dynamic_cast<GameMode *>(GetMode());
			if ( mode == nullptr ) {
				return;
			}

			Actor *actor = mode->GetPossessedActor();
			if ( actor == nullptr ) {
				return;
			}

			PLVector3 forward = actor->GetForward();
			PLVector3 pos = actor->GetPosition();
			defaultCamera->SetPosition( pos.x + forward.x * -500.0f, pos.y + 500.0f, pos.z + forward.z * -500.0f );
			break;
		}

		case CameraMode::FOLLOW: {
			GameMode *mode = dynamic_cast<GameMode *>(GetMode());
			if ( mode == nullptr ) {
				return;
			}

			Actor *actor = mode->GetPossessedActor();
			if ( actor == nullptr ) {
				return;
			}

			PLVector3 forward = actor->GetForward();
			PLVector3 pos = actor->GetPosition();
			defaultCamera->SetPosition( pos.x + forward.x * -500.0f, pos.y + 500.0f, pos.z + forward.z * -500.0f );
			defaultCamera->SetAngles( -25.0f, actor->GetAngles().y, 0.0f );
			break;
		}

		case CameraMode::FLYAROUND:
			break;
	}
}

std::string ohw::GameManager::GetCurrentMapDirectory() const {
	if ( currentMap == nullptr ) {
		return "";
	}

	MapManifest *manifest = currentMap->GetManifest();
	if ( manifest == nullptr ) {
		return "";
	}

	return "maps/" + manifest->filename + "/";
}
