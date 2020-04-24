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

struct modDirectory_t {
	std::string fileName;
	std::string internalName;
	std::string directory;

	// Modification Details
	std::string name; /// Name of the mod
	std::string version; /// The version of the mod
	std::string author; /// Who created the mod
	std::vector<std::string> dependencies; /// Other mods this mod depends on
	bool isVisible = false; /// Whether or not the mod is selectable

	std::vector<PLFileSystemMount*> mountList; /// Pointers to the mounted directory handle
};

typedef std::map<std::string, modDirectory_t> modsMap_t;

const modsMap_t* Mod_GetRegisteredMods();

const modDirectory_t* Mod_GetCurrentMod();

void Mod_RegisterMods();
void Mod_RegisterMod( const char* path );
void Mod_SetMod( const char* name );
