/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include <PL/platform_filesystem.h>

#include "engine.h"
#include "mod_support.h"
#include "script/script_config.h"

using namespace openhow;

/* Mod Management
 *
 * Launching games and other state management
 * of the game as a whole is dealt with here.
 */

modsMap_t modsList;
modDirectory_t* currentModification = nullptr;

/**
 * Pull a list of registered mods which can be displayed
 * for user selection.
 * @return Pointer to modification list.
 */
const modsMap_t* Mod_GetRegisteredMods() {
	return &modsList;
}

static modDirectory_t* Mod_GetManifest( const std::string& name ) {
	auto campaign = modsList.find( name );
	if ( campaign != modsList.end() ) {
		return &campaign->second;
	}

	LogWarn( "Failed to find campaign \"%s\"!\n" );
	return nullptr;
}

static modDirectory_t Mod_LoadManifest( const char* path ) {
	modDirectory_t slot;
	try {
		ScriptConfig config( path );

		slot.name = config.GetStringProperty( "name" );
		slot.version = config.GetStringProperty( "version", "Unknown", true );
		slot.author = config.GetStringProperty( "author", "Unknown", true );
		slot.isVisible = config.GetBooleanProperty( "isVisible", false, true );

		char filename[64];
		snprintf( filename, sizeof( filename ), "%s", plGetFileName( path ) );
		slot.fileName = path;

		char name[32];
		plStripExtension( name, sizeof( name ), plGetFileName( path ) );
		slot.directory = name;

		slot.dependencies = config.GetArrayStrings( "dependencies" );
	} catch ( const std::exception& e ) {
		Error( "Failed to read campaign config, \"%s\"!\n%s\n", path, e.what() );
	}

	return slot;
}

/**
 * Load in the campaign manifest and store it into a buffer.
 * @param path Path to the manifest file.
 */
void Mod_RegisterMod( const char* path ) {
	modDirectory_t mod = Mod_LoadManifest( path );
	modsList.emplace( mod.name, mod );
}

/** Registers all of the campaigns provided under the campaigns
 *  directory.
 */
void Mod_RegisterMods( void ) {
	char path[PL_SYSTEM_MAX_PATH];
	snprintf( path, sizeof( path ), "%s/mods/", u_get_base_path() );
	plScanDirectory( path, "mod", Mod_RegisterMod, false );
}

/** Returns a pointer to the campaign that's currently active.
 *
 * @return Pointer to the current campaign.
 */
const modDirectory_t* Mod_GetCurrentMod( void ) {
	return currentModification;
}

void Mod_SetMod( const char* name ) {
	// Attempt to fetch the manifest, if it doesn't exist then attempt to load it
	modDirectory_t* mod = Mod_GetManifest( name );
	if ( mod == nullptr ) {
		LogInfo( "Mod manifest, \"%s\", wasn't cached on launch... attempting to load!\n", name );

		char path[PL_SYSTEM_MAX_PATH];
		snprintf( path, sizeof( path ), "mods/%s.mod", name );
		if ( plFileExists( path ) ) {
			Mod_RegisterMod( path );
			mod = Mod_GetManifest( name );
		}

		if ( mod == nullptr ) {
			LogWarn( "Mod \"%s\" doesn't exist!\n", name );
			return;
		}
	}

	if ( currentModification == mod ) {
		LogInfo( "Mod is already mounted\n" );
		return;
	}

	// If a modification is already mounted, unmount it
	if ( currentModification != nullptr ) {
		for ( const auto& i : currentModification->mountList ) {
			plClearMountedLocation( i );
		}

		currentModification->mountList.clear();
	}

	u_set_mod_path( mod->directory.c_str() );

	LogInfo( "Mod has been set to \"%s\" successfully!\n", mod->name.c_str() );
}
