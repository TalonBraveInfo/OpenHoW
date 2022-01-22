// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "JsonReader.h"
#include "ShaderManager.h"

static std::map< std::string, ohw::ShaderProgram * > programs;
static ohw::ShaderProgram *fallbackShaderProgram = nullptr;

// For resetting following rebuild
static std::string lastProgramName;

/**
 * Validate the default shader set has been loaded.
 */
static void Shaders_ValidateDefault() {
	const char *defaultShadersStrings[] = {
			"generic_untextured",
			"generic_textured",
			"generic_textured_lit",

			"debug_normals",
			"debug_test",
	};

	for ( unsigned int i = 0; i < PL_ARRAY_ELEMENTS( defaultShadersStrings ); ++i ) {
		ohw::ShaderProgram *program = Shaders_GetProgram( defaultShadersStrings[ i ] );
		if ( program == nullptr ) {
			Error( "Failed to fetch default shader, \"%s\"!\n", defaultShadersStrings[ i ] );
		}

		if ( i == 0 ) {
			fallbackShaderProgram = program;
		}
	}
}

static void Shaders_CacheShaderProgram( const char *path, void *userData ) {
	u_unused( userData );

	const char *fileName = PlGetFileName( path );
	if ( fileName == nullptr ) {
		Warning( "Failed to get filename for shader program, \"%s\"!\n", path );
		return;
	}

	// Provide a copy, minus the extension
	std::string shortFileName( fileName, 0, strlen( fileName ) - 8 );

	try {
		JsonReader config( path );

		config.EnterChildNode( "gl3" );

		std::string vertPath = config.GetStringProperty( "vertPath" );
		std::string fragPath = config.GetStringProperty( "fragPath" );

		if ( vertPath.empty() || fragPath.empty() ) {
			// GetStringProperty throws warnings for this case, so just return
			return;
		}

		ohw::ShaderProgram *program = new ohw::ShaderProgram( vertPath, fragPath );
		programs.insert( std::pair< std::string, ohw::ShaderProgram * >( shortFileName, program ) );
	} catch ( const std::exception &error ) {
		Warning( "Failed to register shader program (%s)!\n", error.what() );
	}
}

static void Shaders_ClearPrograms() {
	for ( const auto &program : programs ) {
		program.second->Disable();
		delete program.second;
	}

	programs.clear();
}

static void Shaders_CachePrograms() {
	Shaders_ClearPrograms();

	PlScanDirectory( "shaders", "program", Shaders_CacheShaderProgram, false, nullptr );

	Shaders_ValidateDefault();
}

static void Cmd_ListShaderPrograms( unsigned int argc, char *argv[] ) {
	u_unused( argc );
	u_unused( argv );

	std::string list = "\n";
	for ( const auto &program : programs ) {
		list += program.first + "\n";
	}

	Print( "%s\n", list.c_str() );
}

static void Cmd_RebuildShaderProgramCache( unsigned int argc, char *argv[] ) {
	u_unused( argc );
	u_unused( argv );

	Shaders_CachePrograms();
}

static void Cmd_RebuildShaderPrograms( unsigned int argc, char *argv[] ) {
	u_unused( argc );
	u_unused( argv );

	for ( const auto &program : programs ) {
		try {
			program.second->Rebuild();
		} catch ( const std::exception &exception ) {
			Warning( "Failed to rebuild shader program, \"%s\" (%s)!\n", program.first.c_str(), exception.what() );
		}
	}

	Shaders_SetProgramByName( lastProgramName );
}

static void Cmd_RebuildShaderProgram( unsigned int argc, char *argv[] ) {
	if ( argc == 0 ) {
		Warning( "Please provide the name of the shader program you wish to rebuild!\n" );
		return;
	} else if ( argc > 1 ) {
		Warning( "Too many arguments, please provide only the name of the shader program you wish to rebuild!\n" );
		return;
	}

	const char *shaderProgramArg = argv[ 0 ];
	if ( shaderProgramArg == NULL ) {
		Warning( "Invalid argument provided, please try again!\n" );
		return;
	}

	ohw::ShaderProgram *shaderProgram = Shaders_GetProgram( shaderProgramArg );
	if ( shaderProgram == nullptr ) {
		return;
	}

	try {
		shaderProgram->Rebuild();
	} catch ( const std::exception &exception ) {
		Warning( "Failed to rebuild shader program, \"%s\" (%s)!\n", shaderProgramArg, exception.what() );
	}
}

void Shaders_Initialize() {
	PlRegisterConsoleCommand( "ListShaderPrograms", Cmd_ListShaderPrograms, "Lists all of the cached shader programs" );
	PlRegisterConsoleCommand( "RebuildShaderPrograms", Cmd_RebuildShaderPrograms, "Rebuild all shader programs" );
	PlRegisterConsoleCommand( "RebuildShaderProgram", Cmd_RebuildShaderProgram, "Rebuild specified shader program" );
	PlRegisterConsoleCommand( "RebuildShaderProgramCache", Cmd_RebuildShaderProgramCache, "Rebuild shader program cache" );

	Shaders_CachePrograms();
}

void Shaders_Shutdown() {
	Shaders_ClearPrograms();
}

ohw::ShaderProgram *Shaders_GetProgram( const std::string &name ) {
	const auto &i = programs.find( name );
	if ( i == programs.end() ) {
		Warning( "Failed to find shader program, \"%s\"!\n", name.c_str() );
		return nullptr;
	}

	return i->second;
}

void Shaders_SetProgramByName( const std::string &name ) {
	ohw::ShaderProgram *shaderProgram = Shaders_GetProgram( name );
	if ( shaderProgram == nullptr ) {
		shaderProgram = fallbackShaderProgram;
	}

	shaderProgram->Enable();
}

ohw::ShaderProgram::ShaderProgram( const std::string &vertPath, const std::string &fragPath ) {
	shaderProgram = PlgCreateShaderProgram();
	if ( shaderProgram == nullptr ) {
		throw std::runtime_error( PlGetError() );
	}

	try {
		RegisterShaderStage( vertPath.c_str(), PLG_SHADER_TYPE_VERTEX );
		RegisterShaderStage( fragPath.c_str(), PLG_SHADER_TYPE_FRAGMENT );
	} catch ( ... ) {
		PlgDestroyShaderProgram( shaderProgram, true );
		throw;
	}

	PlgLinkShaderProgram( shaderProgram );

	this->vertPath = vertPath;
	this->fragPath = fragPath;
}

ohw::ShaderProgram::~ShaderProgram() {
	PlgDestroyShaderProgram( shaderProgram, true );
}

void ohw::ShaderProgram::Rebuild() {
	PLGShaderProgram *newShaderProgram = PlgCreateShaderProgram();
	if ( newShaderProgram == nullptr ) {
		throw std::runtime_error( PlGetError() );
	}

	PLGShaderProgram *oldProgram = shaderProgram;
	shaderProgram = newShaderProgram;

	try {
		RegisterShaderStage( vertPath.c_str(), PLG_SHADER_TYPE_VERTEX );
		RegisterShaderStage( fragPath.c_str(), PLG_SHADER_TYPE_FRAGMENT );
	} catch ( const std::exception &exception ) {
		PlgDestroyShaderProgram( shaderProgram, true );
		shaderProgram = oldProgram;
		throw;
	}

	PlgLinkShaderProgram( shaderProgram );

	PlgDestroyShaderProgram( oldProgram, true );
}

void ohw::ShaderProgram::Enable() {
	PlgSetShaderProgram( shaderProgram );
}

void ohw::ShaderProgram::Disable() {
	PlgSetShaderProgram( nullptr );
}

void ohw::ShaderProgram::RegisterShaderStage( const char *path, PLGShaderStageType type ) {
	if ( !PlgRegisterShaderStageFromDisk( shaderProgram, path, type ) ) {
		throw std::runtime_error( PlGetError() );
	}
}
