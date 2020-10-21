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

#include "script/JsonReader.h"

/* Mod Management
 *
 * Launching games and other state management
 * of the game as a whole is dealt with here.
 */

ohw::ModManager::ModManager() {
	plScanDirectory( "mods", "mod", RegisterMod, false, this );
}

void ohw::ModManager::Mount( const char *name ) {
	// Attempt to fetch the manifest, if it doesn't exist then attempt to load it
	ModDescription *mod = GetModDescription( name );
	if ( mod == nullptr ) {
		Print( "Mod manifest, \"%s\", wasn't cached on launch... attempting to load!\n", name );

		char path[PL_SYSTEM_MAX_PATH];
		snprintf( path, sizeof( path ), "mods/%s.mod", name );
		if ( plFileExists( path ) ) {
			RegisterMod( path, this );
			mod = GetModDescription( name );
		}

		if ( mod == nullptr ) {
			Warning( "Mod \"%s\" doesn't exist!\n", name );
			return;
		}
	}

	if ( myCurrentMod == mod ) {
		Print( "Mod already mounted!\n" );
		return;
	}

	// Generate a list of directories to mount based on the dependencies
	std::set< std::string > dirSet;
	FetchDependencies( mod, dirSet );
	dirSet.emplace( mod->directory );

	Unmount();

	// Now attempt to mount everything
	myCurrentMod = mod;
	for ( const auto &i : dirSet ) {
		char mountPath[PL_SYSTEM_MAX_PATH];
		snprintf( mountPath, sizeof( mountPath ), "mods/%s", i.c_str() );
		PLFileSystemMount *mount = plMountLocation( mountPath );
		if ( mount == nullptr ) {
			Warning( "Failed to mount location, \"%s\" (%s)!\n", i.c_str(), plGetError() );
			continue;
		}

		Print( " Mounted location \"%s\"\n", mountPath );
	}

	Print( "Mod has been set to \"%s\"\n", mod->name.c_str() );
}

void ohw::ModManager::Unmount() {
	// If a modification is already mounted, unmount it
	if ( myCurrentMod != nullptr ) {
		for ( const auto &i : myCurrentMod->mountList ) {
			plClearMountedLocation( i );
		}

		myCurrentMod->mountList.clear();
	}

	// Clear out all the content we've loaded, we'll need to load all our major dependencies after
	GetApp()->resourceManager->ClearAllResources( true );
}

/**
 * Load in the mod manifest and store it into a buffer.
 * @param path Path to the manifest file.
 */
void ohw::ModManager::RegisterMod( const char *path, void *userData ) {
	ohw::ModManager *modManager = static_cast<ModManager *>(userData);

	ModDescription mod = LoadDescription( path );
	modManager->myModsMap.emplace( mod.internalName, mod );
}

void ohw::ModManager::FetchDependencies( ModDescription *modDescription, DirectorySet &output ) {
	const auto &dir = output.find( modDescription->directory );
	if ( dir != output.end() ) {
		Print( "%s is already mounted, skipping\n", modDescription->directory.c_str() );
		return;
	}

	for ( const auto &i : modDescription->dependencies ) {
		const char *name = i.c_str();
		ModDescription *dependency = GetModDescription( name );
		if ( dependency == nullptr ) {
			Warning( "Failed to fetch dependency for mod, \"%s\"!\n", name );
			return;
		}

		if ( !dependency->dependencies.empty() ) {
			FetchDependencies( dependency, output );
		}

		output.emplace( dependency->directory );
	}
}

ohw::ModManager::ModDescription *ohw::ModManager::GetModDescription( const char *name ) {
	auto campaign = myModsMap.find( name );
	if ( campaign != myModsMap.end() ) {
		return &campaign->second;
	}

	Warning( "Failed to find mod \"%s\"!\n" );
	return nullptr;
}

ohw::ModManager::ModDescription ohw::ModManager::LoadDescription( const char *path ) {
	Print( "Loading manifest \"%s\"...\n", path );

	ModDescription modDescription;
	try {
		JsonReader config( path );

		modDescription.name = config.GetStringProperty( "name" );
		modDescription.version = config.GetStringProperty( "version", "Unknown", true );
		modDescription.author = config.GetStringProperty( "author", "Unknown", true );
		modDescription.isVisible = config.GetBooleanProperty( "isVisible", false, true );

		char filename[64];
		snprintf( filename, sizeof( filename ), "%s", plGetFileName( path ) );
		modDescription.fileName = path;
		modDescription.internalName = std::string( filename, strlen( filename ) - 4 );
		modDescription.directory = modDescription.internalName + "/";

		modDescription.dependencies = config.GetArrayStrings( "dependencies" );
	} catch ( const std::exception &e ) {
		Error( "Failed to read mod config, \"%s\"!\n%s\n", path, e.what() );
	}

	return modDescription;
}
