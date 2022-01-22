// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include <duktape.h>
#include <sstream>

#include "App.h"
#include "JsonReader.h"

#define LogMissingProperty( P )   Warning("Failed to get JSON property \"%s\"!\n", (P))
#define LogInvalidArray( P )      Warning("Invalid JSON array for property \"%s\"!\n", (P))

JsonReader::JsonReader( const std::string &path ) : JsonReader() {
	if ( path.empty() ) {
		throw std::runtime_error( "Empty path for config, aborting!\n" );
	}

	PLFile *filePtr = PlOpenFile( path.c_str(), false );
	if ( filePtr == nullptr ) {
		throw std::runtime_error( "Failed to load file!\n" );
	}

	size_t sz = PlGetFileSize( filePtr );
	if ( sz == 0 ) {
		PlCloseFile( filePtr );
		throw std::runtime_error( "Failed to load file, empty config!\n" );
	}

	std::vector<char> buf( sz + 1 );
	PlReadFile( filePtr, buf.data(), sizeof( char ), sz );
	buf[ sz ] = '\0';
	PlCloseFile( filePtr );
	ParseBuffer( buf.data() );
}

JsonReader::JsonReader() {
	if ( ( ctx_ = duk_create_heap_default() ) == nullptr ) {
		throw std::bad_alloc();
	}
}

JsonReader::~JsonReader() {
	if ( ctx_ != nullptr ) {
		duk_destroy_heap( static_cast<duk_context *>(ctx_) );
	}
}

std::string JsonReader::GetStringProperty( const std::string &property, const std::string &def, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);

	const char *p = property.c_str();
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

int JsonReader::GetIntegerProperty( const std::string &property, int def, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);

	const char *p = property.c_str();
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

bool JsonReader::GetBooleanProperty( const std::string &property, bool def, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);

	const char *p = property.c_str();
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

float JsonReader::GetFloatProperty( const std::string &property, float def, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);

	const char *p = property.c_str();
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
std::istream &expect( std::istream &in ) {
	if ( in.peek() == C ) {
		in.ignore();
	} else {
		in.setstate( std::ios_base::failbit );
	}
	return in;
}

hei::Colour JsonReader::GetColourProperty( const std::string &property, const hei::Colour &def, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);

	const char *p = property.c_str();
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

	return hei::Colour( r, g, b, a );
}

PLVector4 JsonReader::GetVector4Property( const std::string &property, PLVector4 def, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);

	const char *p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return def;
	}

	std::string str = duk_safe_to_string( context, -1 );
	duk_pop( context );

	PLVector4 out;
	std::stringstream stream( str );
	stream >> out.x >> expect<' '> >> out.y >> expect<' '> >> out.z >> expect< ' ' > >> out.w;
	if ( stream.rdstate() & std::stringstream::failbit ) {
		throw std::runtime_error( "Failed to parse entirety of vector from JSON property, \"" + property + "\"!\n" );
	}

	return out;
}

PLVector3 JsonReader::GetVector3Property( const std::string &property, PLVector3 def, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);

	const char *p = property.c_str();
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

unsigned int JsonReader::GetArrayLength( const std::string &property, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);

	if ( !property.empty() ) {
		const char *p = property.c_str();
		if ( !duk_get_prop_string( context, -1, p ) ) {
			duk_pop( context );
			if ( !silent ) {
				LogMissingProperty( p );
			}
			return 0;
		}
	}

	if ( !duk_is_array( context, -1 ) ) {
		if ( !property.empty() ) {
			duk_pop( context );
		}
		if ( !silent ) {
			Warning( "Invalid array node!\n" );
		}
		return 0;
	}

	duk_size_t len = duk_get_length( context, -1 );

	if ( !property.empty() ) {
		duk_pop( context );
	}

	return static_cast<unsigned int>(len);
}

std::string JsonReader::GetArrayStringProperty( const std::string &property, unsigned int index ) {
	auto *context = static_cast<duk_context *>(ctx_);

	const char *p = property.c_str();
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
		Warning( "Invalid index, %d (%d), in array!\n", index, length );
		return "";
	}

	duk_get_prop_string( context, -1, p );
	duk_get_prop_index( context, -1, index );
	const char *str = duk_get_string( context, index );
	duk_pop( context );

	return str;
}

std::vector<std::string> JsonReader::GetArrayStrings( const std::string &property, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);

	const char *p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return {};
	}

	if ( !duk_is_array( context, -1 ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return {};
	}

	duk_size_t length = duk_get_length( context, -1 );

	std::vector<std::string> strings;
	strings.reserve( length );

	for ( size_t i = 0; i < length; ++i ) {
		duk_get_prop_index( context, -1, i );
		const char *c = duk_get_string( context, -1 );
		u_assert( c != nullptr, "Null string passed by duk_get_string!\n" );
		strings.emplace_back( c );
		duk_pop( context );
	}

	duk_pop( context );

	return strings;
}

void JsonReader::ParseBuffer( const char *buf ) {
	if ( buf == nullptr ) {
		Error( "Invalid buffer length!\n" );
	}

	auto *context = static_cast<duk_context *>(ctx_);
	duk_push_string( context, buf );
	duk_json_decode( context, -1 );
}

bool JsonReader::EnterChildNode( const std::string &property, bool silent ) {
	auto *context = static_cast<duk_context *>(ctx_);
	const char *p = property.c_str();
	if ( !duk_get_prop_string( context, -1, p ) ) {
		duk_pop( context );
		if ( !silent ) {
			LogMissingProperty( p );
		}
		return false;
	}

	return true;
}

void JsonReader::EnterChildNode( unsigned int index ) {
	auto *context = static_cast<duk_context *>(ctx_);
	if ( !duk_is_array( context, -1 ) ) {
		duk_pop( context );
		Warning( "Node is not an array!\n" );
		return;
	}

	duk_size_t length = duk_get_length( context, -1 );
	if ( index >= length ) {
		Warning( "Invalid index, %d (%d), in array!\n", index, length );
		return;
	}

	duk_get_prop_index( context, -1, index );
}

void JsonReader::LeaveChildNode() {
	auto *context = static_cast<duk_context *>(ctx_);
	duk_pop( context );
}

std::list<std::string> JsonReader::GetObjectKeys() {
	std::list<std::string> lst;

	auto *context = static_cast<duk_context *>(ctx_);
	duk_enum( context, -1, 0 );
	while ( duk_next( context, -1, 1 ) ) {
		lst.emplace_back( duk_safe_to_string( context, -2 ) );
		duk_pop_2( context );
	}
	duk_pop( context );

	return lst;
}
