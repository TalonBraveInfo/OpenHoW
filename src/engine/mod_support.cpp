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

#include <PL/platform_filesystem.h>

#include "engine.h"
#include "mod_support.h"
#include "script/json_reader.h"

using namespace ohw;

/* Mod Management
 *
 * Launching games and other state management
 * of the game as a whole is dealt with here.
 */

modsMap_t modsList;
ModDirectory* currentModification = nullptr;

/**
 * Pull a list of registered mods which can be displayed
 * for user selection.
 * @return Pointer to modification list.
 */
const modsMap_t* Mod_GetRegisteredMods() {
	return &modsList;
}

static ModDirectory* Mod_GetManifest( const std::string& name ) {
	auto campaign = modsList.find( name );
	if ( campaign != modsList.end() ) {
		return &campaign->second;
	}

	LogWarn( "Failed to find mod \"%s\"!\n" );
	return nullptr;
}

static ModDirectory Mod_LoadManifest( const char* path ) {
	LogInfo( "Loading manifest \"%s\"...\n", path );

	ModDirectory slot;
	try {
		JsonReader config( path );

		slot.name = config.GetStringProperty( "name" );
		slot.version = config.GetStringProperty( "version", "Unknown", true );
		slot.author = config.GetStringProperty( "author", "Unknown", true );
		slot.isVisible = config.GetBooleanProperty( "isVisible", false, true );

		char filename[64];
		snprintf( filename, sizeof( filename ), "%s", plGetFileName( path ) );
		slot.fileName = path;
		slot.internalName = std::string( filename, strlen( filename ) - 4 );
		slot.directory = slot.internalName + "/";

		slot.dependencies = config.GetArrayStrings( "dependencies" );
	} catch ( const std::exception& e ) {
		Error( "Failed to read mod config, \"%s\"!\n%s\n", path, e.what() );
	}

	return slot;
}

/**
 * Load in the mod manifest and store it into a buffer.
 * @param path Path to the manifest file.
 */
void Mod_RegisterMod( const char* path, void *userData ) {
	u_unused( userData );
	ModDirectory mod = Mod_LoadManifest( path );
	modsList.emplace( mod.internalName, mod );
}

/**
 * Registers all of the mods provided under the mods directory.
 */
void Mod_RegisterMods() {
	plScanDirectory( "mods", "mod", Mod_RegisterMod, false, nullptr );
}

/**
 * Returns a pointer to the mod that's currently active.
 * @return Pointer to the current mod.
 */
const ModDirectory* Mod_GetCurrentMod() {
	return currentModification;
}

/**
 * Unmounts the specified modification.
 */
static void Mod_Unmount( ModDirectory* mod ) {
	// If a modification is already mounted, unmount it
	if ( mod != nullptr ) {
		for ( const auto& i : mod->mountList ) {
			plClearMountedLocation( i );
		}

		mod->mountList.clear();
	}

	// Clear out all the content we've loaded, we'll need to load all our major dependencies after
	Engine::Resource()->ClearAllResources( true );
}

void Mod_FetchDependencies( ModDirectory* mod, std::set<std::string>& dirSet ) {
	const auto& dir = dirSet.find( mod->directory );
	if ( dir != dirSet.end() ) {
		LogInfo( "%s is already mounted, skipping\n", mod->directory.c_str() );
		return;
	}

	for ( const auto& i : mod->dependencies ) {
		ModDirectory* dependency = Mod_GetManifest( i );
		if ( dependency == nullptr ) {
			LogWarn( "Failed to fetch dependency for mod, \"%s\"!\n", i.c_str() );
			return;
		}

		if ( !dependency->dependencies.empty() ) {
			Mod_FetchDependencies( dependency, dirSet );
		}

		dirSet.emplace( dependency->directory );
	}
}

void Mod_SetMod( const char* name ) {
	// Attempt to fetch the manifest, if it doesn't exist then attempt to load it
	ModDirectory* mod = Mod_GetManifest( name );
	if ( mod == nullptr ) {
		LogInfo( "Mod manifest, \"%s\", wasn't cached on launch... attempting to load!\n", name );

		char path[PL_SYSTEM_MAX_PATH];
		snprintf( path, sizeof( path ), "mods/%s.mod", name );
		if ( plFileExists( path ) ) {
			Mod_RegisterMod( path, nullptr );
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

	// Generate a list of directories to mount based on the dependencies
	std::set<std::string> dirSet;
	Mod_FetchDependencies( mod, dirSet );
	dirSet.emplace( mod->directory );

	// Now attempt to mount everything
	for ( const auto& i : dirSet ) {
		char mountPath[PL_SYSTEM_MAX_PATH];
		snprintf( mountPath, sizeof( mountPath ), "mods/%s", i.c_str() );
		PLFileSystemMount* mount = plMountLocation( mountPath );
		if ( mount == nullptr ) {
			Mod_Unmount( mod );
			LogWarn( "Failed to mount location, \"%s\" (%s)!\n", i.c_str(), plGetError() );
			return;
		}
	}

	if ( currentModification != nullptr ) {
		Mod_Unmount( currentModification );
	}

	currentModification = mod;

	LogInfo( "Mod has been set to \"%s\" successfully!\n", mod->name.c_str() );
}
