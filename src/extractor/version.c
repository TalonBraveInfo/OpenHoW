/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
 * Copyright (C) 2017 Daniel Collins <solemnwarning@solemnwarning.net>
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

#include "extractor.h"

const char *region_idents[ MAX_REGIONS ] = {
        "eng", "fre", "ger", "ita", "rus", "spa" };

VersionInfo version_info = {
        .platform = PLATFORM_UNKNOWN,
        .region = REGION_UNKNOWN };

void CheckGameVersion( const char *path ) {
	Print( "checking game version...\n" );

	char fcheck[ PL_SYSTEM_MAX_PATH ];
	snprintf( fcheck, sizeof( fcheck ), "%s/system.cnf", path );
	if ( PlFileExists( fcheck ) ) {
		Print( "Detected system.cnf, assuming PSX version\n" );
		version_info.platform = PLATFORM_PSX;

#if 0
		// NTSC-U:
		// - SLUS-01195  HOGS OF WAR                          [E]
		// PAL:
		// - SLES-01041  HOGS OF WAR                          [E]
		// - SLES-02769  HOGS OF WAR                          [I]
		// - SLES-02767  HOGS OF WAR [FRONTSCHWEINE]          [G]
		// - SLES-02766  HOGS OF WAR [LES COCHONS DE GUERRE]  [F]
		// - SLES-02768  HOGS OF WAR [MARRANOS EN GUERRA]     [S]

		snprintf(fcheck, sizeof(fcheck), "%s/SLUS_011.95", path);
		if (plFileExists(fcheck)) {
		  //version_info.platform = PLATFORM_PSX_NTSC?;
		  version_info.region = REGION_ENG;
		}

		snprintf(fcheck, sizeof(fcheck), "%s/SLES_010.95", path);
		if (plFileExists(fcheck)) {
		  //version_info.platform = PLATFORM_PSX_PAL?;
		  version_info.region = REGION_ENG;
		}

		snprintf(fcheck, sizeof(fcheck), "%s/SLES_027.66", path);
		if (plFileExists(fcheck)) {
		  //version_info.platform = PLATFORM_PSX_PAL?;
		  version_info.region = REGION_FRE;
		}

		snprintf(fcheck, sizeof(fcheck), "%s/SLES_027.67", path);
		if (plFileExists(fcheck)) {
		  //version_info.platform = PLATFORM_PSX_PAL?;
		  version_info.region = REGION_GER;
		}

		snprintf(fcheck, sizeof(fcheck), "%s/SLES_027.68", path);
		if (plFileExists(fcheck)) {
		  //version_info.platform = PLATFORM_PSX_PAL?;
		  version_info.region = REGION_SPA;
		}

		snprintf(fcheck, sizeof(fcheck), "%s/SLES_027.69", path);
		if (plFileExists(fcheck)) {
		  //version_info.platform = PLATFORM_PSX_PAL?;
		  version_info.region = REGION_ITA;
		}

		if (version_info.region == REGION_UNKNOWN) {
		  LogWarn("Failed to find any known PSX-EXE file, unable to determine region!\n");
		  return;
		}
#else
		// Alternative approach reading the language.lng file.
		// Language ids:
		// 0 = ENG_GB
		// 1 = ENG_US
		// 2 = FRE (no proof)
		// 3 = GER
		// 4 = SPA (no proof)
		// 5 = ITA (no proof)

		PLFile *languageFile = PlOpenFile( path, false );
		if ( languageFile == NULL ) {
			Warning( "Failed to load \"%s\", aborting (%s)!\n", path, PlGetError() );
			return;
		}

		uint32_t languageId;
		if ( PlReadFile( languageFile, &languageId, sizeof( uint32_t ), 1 ) != 1 ) {
			Warning( "Failed to get language code, \"%s\" (%s)!\n", path, PlGetError() );
			return;
		}

		if ( languageId == 0 || languageId == 1 ) {
			version_info.region = REGION_ENG;
		} else if ( languageId == 2 ) {
			version_info.region = REGION_FRE;
		} else if ( languageId == 3 ) {
			version_info.region = REGION_GER;
		} else if ( languageId == 4 ) {
			version_info.region = REGION_SPA;
		} else if ( languageId == 5 ) {
			version_info.region = REGION_ITA;
		} else {
			Warning( "Language id (%d) is unknown, unable to determine region!\n", languageId );
			return;
		}
#endif
	}

	snprintf( fcheck, sizeof( fcheck ), "%s/Data/foxscale.d3d", path );
	if ( !PlFileExists( fcheck ) ) {
		Warning( "Failed to find foxscale.d3d, unable to determine platform!\n" );
		return;
	}

	Print( "Detected Data/foxscale.d3d, assuming PC version\n" );
	version_info.platform = PLATFORM_PC;

	/* todo: need better method to determine this */
	/* todo: use checksum, not filesize!!! */
	snprintf( fcheck, sizeof( fcheck ), "%s/Language/Text/fetext.bin", path );
	unsigned int fetext_size = PlGetLocalFileSize( fcheck );
	snprintf( fcheck, sizeof( fcheck ), "%s/Language/Text/gtext.bin", path );
	unsigned int gtext_size = PlGetLocalFileSize( fcheck );
	if ( fetext_size == 8997 && gtext_size == 4518 ) {
		version_info.region = REGION_GER;
	} else if ( fetext_size == 8102 && gtext_size == 4112 ) {
		version_info.region = REGION_ENG;
	} else if ( fetext_size == 8391 && gtext_size == 4513 ) {
		version_info.region = REGION_RUS;
	}

	snprintf( fcheck, sizeof( fcheck ), "%s/MUSIC/Track02.ogg", path );
	if ( PlFileExists( fcheck ) ) {
		Print( "Detected MUSIC/Track02.ogg, assuming GOG version\n" );
		version_info.platform = PLATFORM_PC_DIGITAL;
	}

	Print( "platform=%d, region=%d\n", version_info.platform, version_info.region );
}
