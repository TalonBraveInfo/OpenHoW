// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include <map>
#include <set>

namespace ohw {
	class ModManager {
	public:
		ModManager();

		void Mount( const char *name );
		void Unmount();

		struct ModDescription {
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
		typedef std::map< std::string, ModDescription > ModsMap;
		inline const ModsMap *GetAvailableMods() const {
			return &myModsMap;
		}

		inline const ModDescription *GetCurrentModDescription() const {
			return myCurrentMod;
		}

	private:
		static void RegisterMod( const char *path, void *userData );

		typedef std::set<std::string> DirectorySet;
		void FetchDependencies( ModDescription *modDescription, DirectorySet &output );

		ModDescription *GetModDescription( const char *name );
		static ModDescription LoadDescription( const char *path );

		ModsMap myModsMap;
		ModDescription *myCurrentMod{ nullptr };
	};
}
