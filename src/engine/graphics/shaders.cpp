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

#include "../engine.h"
#include "../script/script_config.h"
#include "shaders.h"

static std::map<std::string, hwShaderProgram*> programs;
static hwShaderProgram* fallbackShaderProgram = nullptr;

// For resetting following rebuild
static std::string lastProgramName;

/**
 * Validate the default shader set has been loaded.
 */
static void Shaders_ValidateDefault() {
	const char* defaultShaders[] = {
		"generic_untextured",
		"generic_textured",
		"generic_textured_lit",
	};

	for ( unsigned int i = 0; i < plArrayElements( defaultShaders ); ++i ) {
		hwShaderProgram* program = Shaders_GetProgram( defaultShaders[ i ] );
		if ( program == nullptr ) {
			Error( "Failed to fetch default shader, \"%s\"!\n", defaultShaders[ i ] );
		}

		if ( i == 0 ) {
			fallbackShaderProgram = program;
		}
	}
}

static void Shaders_CacheShaderProgram( const char* path ) {
	const char* fileName = plGetFileName( path );
	if ( fileName == nullptr ) {
		LogWarn( "Failed to get filename for shader program, \"%s\"!\n", path );
		return;
	}

	// Provide a copy, minus the extension
	std::string shortFileName( fileName, 0, strlen( fileName ) - 8 );

	try {
		ScriptConfig config( path );

		config.EnterChildNode( "gl3" );

		std::string vertPath = config.GetStringProperty( "vertPath" );
		std::string fragPath = config.GetStringProperty( "fragPath" );

		if ( vertPath.empty() || fragPath.empty()) {
			// GetStringProperty throws warnings for this case, so just return
			return;
		}

		hwShaderProgram* program = new hwShaderProgram( vertPath, fragPath );
		programs.insert( std::pair<std::string, hwShaderProgram*>( shortFileName, program ));
	} catch ( const std::exception& error ) {
		LogWarn( "Failed to register shader program, %s!\n", error.what());
	}
}

static void Shaders_ClearPrograms() {
	for ( const auto& program : programs ) {
		program.second->Disable();
		delete program.second;
	}

	programs.clear();
}

static void Shaders_CachePrograms() {
	Shaders_ClearPrograms();

	char scanPath[PL_SYSTEM_MAX_PATH];
	snprintf( scanPath, sizeof( scanPath ), "%s/shaders", u_get_base_path());
	plScanDirectory( scanPath, "program", Shaders_CacheShaderProgram, false );

	// Recursive searches through dependencies... This needs revisiting!
	const char* modPath = u_get_mod_path();
	if ( modPath[ 0 ] != '\0' ) {
		snprintf( scanPath, sizeof( scanPath ), "%s/shaders", u_get_full_path());
		plScanDirectory( scanPath, "program", Shaders_CacheShaderProgram, false );
	}

	Shaders_ValidateDefault();
}

static void Cmd_RebuildShaderPrograms( unsigned int argc, char* argv[] ) {
	Shaders_CachePrograms();
}

static void Cmd_RebuildShaderProgram( unsigned int argc, char* argv[] ) {
	u_unused( argc );
	u_unused( argv );

	try {
		for ( const auto& program : programs ) {
			program.second->Rebuild();
		}
	} catch ( ... ) {
		Error( "Failed to rebuild shader programs, check logs for details!\n" );
	}

	Shaders_SetProgramByName( lastProgramName );
}

void Shaders_Initialize() {
	plRegisterConsoleCommand( "rebuildShaderPrograms", Cmd_RebuildShaderPrograms, "Rebuild all shader programs" );
	plRegisterConsoleCommand( "rebuildShaderProgram", Cmd_RebuildShaderProgram, "Rebuild specified shader program" );

	Shaders_CachePrograms();
}

void Shaders_Shutdown() {
	Shaders_ClearPrograms();
}

hwShaderProgram* Shaders_GetProgram( const std::string& name ) {
	const auto& i = programs.find( name );
	if ( i == programs.end()) {
		LogWarn( "Failed to find shader program, \"%s\"!\n", name.c_str());
		return nullptr;
	}

	return i->second;
}

void Shaders_SetProgram( hwShaderProgram* shaderProgram ) {
	shaderProgram->Enable();
}

void Shaders_SetProgramByName( const std::string& name ) {
	hwShaderProgram* shaderProgram = Shaders_GetProgram( name );
	if ( shaderProgram == nullptr ) {
		shaderProgram = fallbackShaderProgram;
	}

	Shaders_SetProgram( shaderProgram );
}

hwShaderProgram::hwShaderProgram( const std::string& vertPath, const std::string& fragPath ) {
	shaderProgram = plCreateShaderProgram();
	if ( shaderProgram == nullptr ) {
		throw std::runtime_error( plGetError());
	}

	try {
		RegisterShaderStage( vertPath.c_str(), PL_SHADER_TYPE_VERTEX );
		RegisterShaderStage( fragPath.c_str(), PL_SHADER_TYPE_FRAGMENT );
	} catch ( ... ) {
		plDestroyShaderProgram( shaderProgram, true );
		throw;
	}

	plLinkShaderProgram( shaderProgram );

	this->vertPath = vertPath;
	this->fragPath = fragPath;
}

hwShaderProgram::~hwShaderProgram() {
	plDestroyShaderProgram( shaderProgram, true );
}

void hwShaderProgram::Rebuild() {
	plDestroyShaderProgram( shaderProgram, true );
	shaderProgram = nullptr;

	shaderProgram = plCreateShaderProgram();
	if ( shaderProgram ) {
		throw std::runtime_error( plGetError());
	}

	RegisterShaderStage( vertPath.c_str(), PL_SHADER_TYPE_VERTEX );
	RegisterShaderStage( fragPath.c_str(), PL_SHADER_TYPE_FRAGMENT );

	plLinkShaderProgram( shaderProgram );
}

void hwShaderProgram::Enable() {
	plSetShaderProgram( shaderProgram );
}

void hwShaderProgram::Disable() {
	plSetShaderProgram( nullptr );
}

void hwShaderProgram::RegisterShaderStage( const char* path, PLShaderType type ) {
	if ( !plRegisterShaderStageFromDisk( shaderProgram, u_find( path ), type )) {
		throw std::runtime_error( plGetError());
	}
}
