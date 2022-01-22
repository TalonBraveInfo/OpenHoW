// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Language.h"
#include "JsonReader.h"

#define LNG_MANIFEST_PATH    "languages.manifest"

ohw::LanguageManager *ohw::LanguageManager::language_manager_;

ohw::LanguageManager::LanguageManager() {
	Print( "Loading " LNG_MANIFEST_PATH "\n" );

	// Load in the languages manifest
	try {
		JsonReader manifest( LNG_MANIFEST_PATH );
		unsigned int num_keys = manifest.GetArrayLength();
		for ( unsigned int i = 0; i < num_keys; ++i ) {
			Index index;
			manifest.EnterChildNode( i );
			index.name = manifest.GetStringProperty( "name" );
			index.key = manifest.GetStringProperty( "key" );
			manifest.LeaveChildNode();
			languages_.insert( std::pair< std::string, Index >( index.key, index ) );
		}
	} catch ( const std::exception &e ) {
		Error( "Failed to load languages manifest, \"%s\"!\n", LNG_MANIFEST_PATH );
	}
}

ohw::LanguageManager::~LanguageManager() = default;

const char *ohw::LanguageManager::GetTranslation( const char *key ) { // todo: UTF-8 pls
	if ( key[ 0 ] != '$' ) {
		return key;
	}

	const char *p = ++key;
	if ( *p == '\0' ) {
		Warning( "Invalid key provided\n" );
		return p;
	}

	if ( current_language == nullptr ) {
		Warning( "No valid language set\n" );
		return p;
	}

	auto i = current_language->keys.find( p );
	if ( i == current_language->keys.end() ) {
		Warning( "Failed to find translation key, %s\n", p );
		return p;
	}

	return i->second.translation.c_str();
}

void ohw::LanguageManager::SetLanguage( const char *key ) {
	if ( current_language != nullptr && current_language->key == key ) {
		return;
	}

	auto language = languages_.find( key );
	if ( language == languages_.end() ) {
		Error( "Failed to find specified language, \"%s\"!\n", key );
	}

	current_language = &language->second;

	std::string filePath = "languages/" + current_language->key + ".language";
	try {
		JsonReader manifest( filePath );
		std::list< std::string > keys = manifest.GetObjectKeys();
		for ( const auto &idx : keys ) {
			current_language->keys.insert( std::pair< std::string, Key >( idx, {
					manifest.GetStringProperty( idx )
			} ) );
		}
	} catch ( const std::exception &e ) {
		Error( "Failed to load language manifest, \"%s\"!\n", filePath.c_str() );
	}
}

void ohw::LanguageManager::SetLanguageCallback( const PLConsoleVariable *var ) {
	LanguageManager::GetInstance()->SetLanguage( var->s_value );
}
