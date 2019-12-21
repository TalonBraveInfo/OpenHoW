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
#include <duktape.h>
#include <sstream>

#include "../engine.h"
#include "script_config.h"

#define LogMissingProperty( P )   LogWarn("Failed to get JSON property \"%s\"!\n", (P))
#define LogInvalidArray( P )      LogWarn("Invalid JSON array for property \"%s\"!\n", (P))

ScriptConfig::ScriptConfig( const std::string& path ) : ScriptConfig() {
	if ( path.empty() ) {
		throw std::runtime_error( "Empty path for config, aborting!\n" );
	}

	PLFile* filePtr = plOpenFile( path.c_str(), false );
	size_t sz = plGetFileSize( filePtr );
	if ( sz == 0 ) {
		plCloseFile( filePtr );
		throw std::runtime_error( "Failed to load file, empty config!\n" );
	}

	std::vector<char> buf( sz + 1 );
	plReadFile( filePtr, buf.data(), sizeof( char ), sz );
	buf[ sz ] = '\0';
	plCloseFile( filePtr );
	ParseBuffer( buf.data() );
}

ScriptConfig::ScriptConfig() {
	if ( ( ctx_ = duk_create_heap_default() ) == nullptr ) {
		throw std::bad_alloc();
	}
}

ScriptConfig::~ScriptConfig() {
	if ( ctx_ != nullptr ) {
		duk_destroy_heap( static_cast<duk_context*>(ctx_) );
	}
}

std::string ScriptConfig::GetStringProperty( const std::string& property, const std::string& def, bool silent ) {
	auto* context = static_cast<duk_context*>(ctx_);

	const char* p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return def;
	}

	std::string str = duk_safe_to_string( context, -1 );
	duk_pop( context );

	return str;
}

int ScriptConfig::GetIntegerProperty( const std::string& property, int def, bool silent ) {
	auto* context = static_cast<duk_context*>(ctx_);

	const char* p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return def;
	}

	int var = duk_to_int32( context, -1 );
	duk_pop( context );

	return var;
}

bool ScriptConfig::GetBooleanProperty( const std::string& property, bool def, bool silent ) {
	auto* context = static_cast<duk_context*>(ctx_);

	const char* p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return def;
	}

	auto var = static_cast<bool>(duk_to_boolean( context, -1 ));
	duk_pop( context );

	return var;
}

float ScriptConfig::GetFloatProperty( const std::string& property, float def, bool silent ) {
	auto* context = static_cast<duk_context*>(ctx_);

	const char* p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return def;
	}

	float var = duk_to_number( context, -1 );
	duk_pop( context );

	return var;
}

// https://stackoverflow.com/a/23305012
template< char C >
std::istream& expect( std::istream& in ) {
	if ( in.peek() == C ) {
		in.ignore();
	} else {
		in.setstate( std::ios_base::failbit );
	}
	return in;
}

PLColour ScriptConfig::GetColourProperty( const std::string& property, PLColour def, bool silent ) {
	auto* context = static_cast<duk_context*>(ctx_);

	const char* p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return def;
	}

	std::string str = duk_safe_to_string( context, -1 );
	duk_pop( context );

	std::stringstream stream( str );
	int r, g, b, a;
	stream >> r >> expect<' '> >> g >> expect<' '> >> b;
	if ( !( stream.rdstate() & std::stringstream::failbit ) ) {
		stream >> expect<' '> >> a;
		if ( stream.rdstate() & std::stringstream::failbit ) {
			// can still ignore alpha channel
			a = 255;
		}
	} else {
		throw std::runtime_error( "Failed to parse entirety of colour from JSON property, \"" + property + "\"!\n" );
	}

	return { r, g, b, a };
}

PLVector3 ScriptConfig::GetVector3Property( const std::string& property, PLVector3 def, bool silent ) {
	auto* context = static_cast<duk_context*>(ctx_);

	const char* p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return def;
	}

	std::string str = duk_safe_to_string( context, -1 );
	duk_pop( context );

	PLVector3 out;
	std::stringstream stream( str );
	stream >> out.x >> expect<' '> >> out.y >> expect<' '> >> out.z;
	if ( stream.rdstate() & std::stringstream::failbit ) {
		throw std::runtime_error( "Failed to parse entirety of vector from JSON property, \"" + property + "\"!\n" );
	}

	return out;
}

unsigned int ScriptConfig::GetArrayLength( const std::string& property ) {
	auto* context = static_cast<duk_context*>(ctx_);

	if ( !property.empty() ) {
		const char* p = property.c_str();
		if ( !duk_get_prop_string( context, -1, p ) ) {
			duk_pop( context );
			LogMissingProperty( p );
			return 0;
		}
	}

	if ( !duk_is_array( context, -1 ) ) {
		if ( !property.empty() ) {
			duk_pop( context );
		}
		LogWarn( "Invalid array node!\n" );
		return 0;
	}

	duk_size_t len = duk_get_length( context, -1 );

	if ( !property.empty() ) {
		duk_pop( context );
	}

	return static_cast<unsigned int>(len);
}

std::string ScriptConfig::GetArrayStringProperty( const std::string& property, unsigned int index ) {
	auto* context = static_cast<duk_context*>(ctx_);

	const char* p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		LogMissingProperty( p );
		return "";
	}

	if ( !duk_is_array( context, -1 ) ) {
		duk_pop( context );
		LogInvalidArray( p );
		return "";
	}

	duk_size_t length = duk_get_length( context, -1 );
	if ( index >= length ) {
		LogWarn( "Invalid index, %d (%d), in array!\n", index, length );
		return "";
	}

	duk_get_prop_string( context, -1, p );
	duk_get_prop_index( context, -1, index );
	const char* str = duk_get_string( context, index );
	duk_pop( context );

	return str;
}

std::vector<std::string> ScriptConfig::GetArrayStrings( const std::string& property ) {
	auto* context = static_cast<duk_context*>(ctx_);

	const char* p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		LogMissingProperty( p );
		return {};
	}

	if ( !duk_is_array( context, -1 ) ) {
		duk_pop( context );
		LogInvalidArray( p );
		return {};
	}

	duk_size_t length = duk_get_length( context, -1 );

	std::vector<std::string> strings;
	strings.reserve( length );

	for ( size_t i = 0; i < length; ++i ) {
		duk_get_prop_index( context, -1, i );
		const char* c = duk_get_string( context, -1 );
		u_assert( c != nullptr, "Null string passed by duk_get_string!\n" );
		strings.emplace_back( c );
		duk_pop( context );
	}

	duk_pop( context );

	return strings;
}

void ScriptConfig::ParseBuffer( const char* buf ) {
	if ( buf == nullptr ) {
		Error( "Invalid buffer length!\n" );
	}

	auto* context = static_cast<duk_context*>(ctx_);
	duk_push_string( context, buf );
	duk_json_decode( context, -1 );
}

void ScriptConfig::EnterChildNode( const std::string& property ) {
	auto* context = static_cast<duk_context*>(ctx_);
	const char* p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		LogMissingProperty( p );
		return;
	}
}

void ScriptConfig::EnterChildNode( unsigned int index ) {
	auto* context = static_cast<duk_context*>(ctx_);
	if ( !duk_is_array( context, -1 ) ) {
		duk_pop( context );
		LogWarn( "Node is not an array!\n" );
		return;
	}

	duk_size_t length = duk_get_length( context, -1 );
	if ( index >= length ) {
		LogWarn( "Invalid index, %d (%d), in array!\n", index, length );
		return;
	}

	duk_get_prop_index( context, -1, index );
}

void ScriptConfig::LeaveChildNode() {
	auto* context = static_cast<duk_context*>(ctx_);
	duk_pop( context );
}

std::list<std::string> ScriptConfig::GetObjectKeys() {
	std::list<std::string> lst;

	auto* context = static_cast<duk_context*>(ctx_);
	duk_enum( context, -1, 0 );
	while ( duk_next( context, -1, 1 ) ) {
		lst.emplace_back( duk_safe_to_string( context, -2 ) );
		duk_pop_2( context );
	}
	duk_pop( context );

	return lst;
}
