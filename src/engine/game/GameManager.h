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

#pragma once

#include "Actor.h"
#include "GameModeInterface.h"

enum class CharacterStatus {
	NONE = -1,
	ALIVE,
	DEAD,
};

/* 
  NONE = -1,
  GRUNT,
  // Heavy Weapons
  GUNNER,
  BOMBARDIER,
  PYROTECHNIC,
  // Engineer
  SAPPER,
  ENGINEER,
  SABOTEUR,
  // Espionage
  SCOUT,
  SNIPER,
  SPY,
  // Medic
  ORDERLY,
  MEDIC,
  SURGEON,
  // High Rank
  COMMANDO,
  HERO,
  ACE,
  LEGEND,
  // Multiplayer
  PARATROOPER,
  GRENADIER,
 */

struct CharacterClass : PropertyOwner {
	CharacterClass() :
			INIT_PROPERTY( identifer, 0 ),
			INIT_PROPERTY( label, 0 ),
			INIT_PROPERTY( model, 0 ),
			INIT_PROPERTY( nextClass, 0 ),
			INIT_PROPERTY( cost, 0, 0 ),
			INIT_PROPERTY( health, 0, 0 ) {}
	CharacterClass( const CharacterClass &src ) :
			COPY_PROPERTY( identifer, src ),
			COPY_PROPERTY( label, src ),
			COPY_PROPERTY( model, src ),
			COPY_PROPERTY( nextClass, src ),
			COPY_PROPERTY( cost, src ),
			COPY_PROPERTY( health, src ) {}

	StringProperty identifer;
	StringProperty label;
	StringProperty model;
	StringProperty nextClass;
	NumericProperty< unsigned int > cost;
	NumericProperty< unsigned int > health;

	struct Item {
		std::string key;
		std::string classname;
		unsigned int quantity{ 0 };
	};
	std::vector< Item > items;
};

struct CharacterSlot : PropertyOwner {
	CharacterSlot() :
			INIT_PROPERTY( portrait, 0 ),
			INIT_PROPERTY( portrait_selected, 0 ),
			INIT_PROPERTY( portrait_wounded, 0 ),
			INIT_PROPERTY( voice_language, 0 ),
			INIT_PROPERTY( voice_set, 0, 0 ) {}
	CharacterSlot( const CharacterSlot &src ) :
			COPY_PROPERTY( portrait, src ),
			COPY_PROPERTY( portrait_selected, src ),
			COPY_PROPERTY( portrait_wounded, src ),
			COPY_PROPERTY( voice_language, src ),
			COPY_PROPERTY( voice_set, src ),
			name( src.name ),
			classname( src.classname ),
			status( src.status ),
			kill_count( src.kill_count ),
			death_count( src.death_count ) {}

	// Static
	StringProperty portrait;           // Face profile
	StringProperty portrait_selected;  // Selected face profile
	StringProperty portrait_wounded;   // Wounded face profile
	StringProperty voice_language;
	NumericProperty< unsigned int > voice_set;

	// Dynamic
	std::string name;                               // Assigned name, e.g. Herman
	const CharacterClass *classname{ nullptr };               // Ace, gunner, sapper etc.
	CharacterStatus status{ CharacterStatus::ALIVE };   // Pig's status (alive / dead)
	unsigned int kill_count{ 0 };                    // Number of other pigs we've killed
	unsigned int death_count{ 0 };                   // Number of times we've died
};

struct PlayerTeam : PropertyOwner {
	PlayerTeam() :
			INIT_PROPERTY( name, 0 ),
			INIT_PROPERTY( description, 0 ),
			INIT_PROPERTY( pig_textures, 0 ),
			INIT_PROPERTY( paper_texture, 0 ),
			INIT_PROPERTY( debrief_texture, 0 ),
			INIT_PROPERTY( voice_set, 0 ) {}
	PlayerTeam( const PlayerTeam &src ) :
			COPY_PROPERTY( name, src ),
			COPY_PROPERTY( description, src ),
			COPY_PROPERTY( pig_textures, src ),
			COPY_PROPERTY( paper_texture, src ),
			COPY_PROPERTY( debrief_texture, src ),
			COPY_PROPERTY( voice_set, src ),
			slots( src.slots ) {}

	StringProperty name;
	StringProperty description;

	// data directories
	StringProperty pig_textures;
	StringProperty paper_texture;
	StringProperty debrief_texture;
	StringProperty voice_set;

	PLColour colour;

	std::array< CharacterSlot, 8 > slots;
};

struct MapManifest {
	std::string filepath; // path to manifest
	std::string filename; // name of the manifest

	std::string name{ "none" };                       // 'BOOT CAMP'
	std::string author{ "none" };                     // creator of the map
	std::string description{ "none" };                //
	std::string tile_directory;
	std::vector< std::string > modes;                 // supported gameplay types
	PLColour ambient_colour{ 255, 255, 255, 255 };    // ambient colour
	// Sky gradient
	PLColour sky_colour_top{ 0, 104, 156 };
	PLColour sky_colour_bottom{ 223, 255, 255 };
	// Sun/lighting properties
	PLColour sun_colour{ 255, 255, 255 };      // directional colour
	float sun_yaw{ 0 }, sun_pitch{ 0 };       // light direction (yaw/angle)
	// Fog
	PLColour fog_colour{ 223, 255, 255, 255 };
	float fog_intensity{ 30.0f };
	float fog_distance{ 100.0f };
	// Misc
	std::string temperature{ "normal" };    // Determines idle animation set. Can be normal/hot/cold
	std::string time{ "day" };              // Determines ambient sound set. Can be day/night
	std::string weather{ "clear" };         // Determines weather particles. Can be clear/rain/snow

	std::string Serialize();
};

struct GameModeDescriptor {
	std::string desired_mode{ "survival" };

	// Mode specific properties, probably provide these
	// via json going forward and then parse via the mode... somehow
	bool select_pig{ false };
	bool sudden_death{ false };
	unsigned int default_health{ 50 };
	unsigned int num_pigs{ 3 };
	unsigned int turn_time{ 45 };
	unsigned int deathmatch_limit{ 5 };
};

typedef std::vector< Player * > PlayerPtrVector;

struct AudioSample;

namespace ohw {
	class Map;
	class Camera;
	class GameManager {
	private:
		GameManager();
		~GameManager();

	public:
		void Tick();

		PL_INLINE Camera *GetActiveCamera() const { return defaultCamera; }

		void StartMode( const std::string &map, const PlayerPtrVector &players, const GameModeDescriptor &descriptor );
		void EndMode();

		void SetupPlayers( const PlayerPtrVector &teams );
		void ClearPlayers();
		Player *GetPlayerByIndex( unsigned int i ) const;
		const PlayerPtrVector &GetPlayers() { return players_; }

		// Map
		void LoadMap( const std::string &name );
		void UnloadMap();

		void CachePersistentData();

		void RegisterTeamManifest( const std::string &path );
		void RegisterClassManifest( const std::string &path );
		void RegisterMapManifest( const std::string &path );
		void RegisterMapManifests();

		typedef std::map< std::string, MapManifest > MapManifestMap;
		MapManifest *GetMapManifest( const std::string &name );
		const MapManifestMap &GetMapManifests() { return mapManifests; };
		MapManifest *CreateManifest( const std::string &name );
		//void SaveManifest(const std::string& name, const MapManifest& manifest);

		typedef std::vector< PlayerTeam > PlayerTeamVector;
		const PlayerTeamVector &GetDefaultTeams() const { return defaultTeams; }

		typedef std::map< std::string, CharacterClass > CharacterClassMap;
		const CharacterClass *GetDefaultClass( const std::string &classIdentifer ) const;

		PL_INLINE Map *GetCurrentMap() const { return currentMap; }
		PL_INLINE IGameMode *GetMode() const { return currentMode; }

		bool IsModeActive();

		PL_INLINE void StepSimulation( unsigned int steps = 1 ) {
			simSteps = steps;
		}

		PL_INLINE void ToggleSimulation( bool paused ) {
			pauseSim = paused;
		}

		std::string GetCurrentMapDirectory() const;

	protected:
		void TickCamera();

	private:
		static void OpenMapCommand( unsigned int argc, char *argv[] );
		static void CreateMapCommand( unsigned int argc, char *argv[] );
		static void ListMapsCommand( unsigned int argc, char **argv );
		static void GiveItemCommand( unsigned int argc, char *argv[] );
		static void KillSelfCommand( unsigned int argc, char **argv );
		static void SpawnModelCommand( unsigned int argc, char **argv );
		static void TeleportCommand( unsigned int argc, char **argv );
		static void FirstPersonCommand( unsigned int argc, char **argv );
		static void FreeCamCommand( unsigned int argc, char **argv );

		bool pauseSim{ false };
		unsigned int simSteps{ 0 };

		/////////////////////////////////////////////////////////////
		// Camera

		enum class CameraMode {
			// Debugging modes

			FLY,          // Fly around
			FIRSTPERSON,  // First person controls

			// Gameplay

			FOLLOW,       // Follows behind a specific entity
			FLYAROUND,
		};

		Camera *defaultCamera{ nullptr };
		CameraMode cameraMode{ CameraMode::FOLLOW };

		/////////////////////////////////////////////////////////////

		Map *currentMap{ nullptr };

		IGameMode *currentMode{ nullptr };

		std::map< std::string, MapManifest > mapManifests;

		PlayerTeamVector defaultTeams;
		CharacterClassMap defaultClasses;

		PlayerPtrVector players_;

#define MAX_AMBIENT_SAMPLES 8
		double ambient_emit_delay_{ 0 };
		const AudioSample *ambient_samples_[MAX_AMBIENT_SAMPLES]{};

		friend class App;
	};
}
