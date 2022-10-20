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

const char *Config_GetUserConfigPath() {
	static std::string config_path;
	if ( config_path.empty() ) {
		char out[ PL_SYSTEM_MAX_PATH ];
		if ( PlGetApplicationDataDirectory( APP_NAME, out, PL_SYSTEM_MAX_PATH ) == nullptr ) {
			Warning( "Failed to get app data directory!\n%s\n", PlGetError() );
			config_path = "./user.config";
		} else {
			config_path = std::string( out ) + "user.config";
		}
	}
	return config_path.c_str();
}

void Config_Save( const char *path ) {
	FILE *fp = fopen( path, "wb" );
	if ( fp == nullptr ) {
		Warning( "failed to write config to \"%s\"!\n", path );
		return;
	}

	fprintf( fp, "{\n" );

	/* get access to the console variables from the platform library */
	size_t num_c;
	PLConsoleVariable **vars;
	PlGetConsoleVariables( &vars, &num_c );

	/* and NOW save them! */
	bool first = true;
	for ( PLConsoleVariable **var = vars; var < num_c + vars; ++var ) {
		if ( !( *var )->archive ) {
			continue;
		}

		if ( !first ) {
			fprintf( fp, ",\n" );
		}

		fprintf( fp, "\t\t\"%s\":\"%s\"", ( *var )->name, ( *var )->value );
		first = false;
	}

	fprintf( fp, "\n}\n" );
	fclose( fp );
}

void Config_Load( const char *path ) {
	try {
		JsonReader config( path );

		size_t num_c;
		PLConsoleVariable **vars;
		PlGetConsoleVariables( &vars, &num_c );

		for ( PLConsoleVariable **var = vars; var < vars + num_c; ++var ) {
			std::string result = config.GetStringProperty( ( *var )->name );
			if ( result.empty() ) {
				continue;
			}

			PlSetConsoleVariable( ( *var ), result.c_str() );
		}
	} catch ( const std::exception &e ) {
		Warning( "Failed to read user config, \"%s\"!\n%s\n", path, e.what() );
	}
}
