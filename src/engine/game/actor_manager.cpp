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

#include "engine.h"
#include "Menu.h"
#include "graphics/ShaderManager.h"
#include "actor_manager.h"
#include "json_reader.h"

/************************************************************/

ActorSet ActorManager::actorsList;
std::vector< Actor * > ActorManager::destructionQueue;
std::map< std::string, ActorManager::actor_ctor_func > ActorManager::actorClassesRegistry
		__attribute__((init_priority (1000)));

Actor *ActorManager::CreateActor( const std::string &identifier, const ActorSpawn &spawnData ) {
	auto spawn = actorSpawnsRegistry.find( identifier );
	if ( spawn == actorSpawnsRegistry.end() ) {
		// TODO: make this throw an error rather than continue...
		Warning( "Failed to find actor in spawn registry \"%s\"!\n", identifier.c_str() );
		return nullptr;
	}

	auto classSpawn = actorClassesRegistry.find( spawn->second.className );
	if ( classSpawn == actorClassesRegistry.end() ) {
		Error( "Invalid class name \"%s\" provided for actor \"%s\"!\n",
		 spawn->second.className.c_str(),
		 spawn->second.identifier.c_str() );
	}

	Actor *actor = classSpawn->second();
	actorsList.insert( actor );

	actor->Deserialize( spawnData );

	return actor;
}

void ActorManager::DestroyActor( Actor *actor ) {
	u_assert( actor != nullptr, "attempted to delete a null actor!\n" );

	// Ensure it's not already queued for destruction
	if ( std::find( destructionQueue.begin(), destructionQueue.end(), actor ) != destructionQueue.end() ) {
		LogDebug( "Attempted to queue actor for deletion twice, ignoring...\n" );
		return;
	}

	// Move it into a queue for destruction
	destructionQueue.push_back( actor );
}

void ActorManager::TickActors() {
	for ( auto const &actor: actorsList ) {
		if ( !actor->IsActivated() ) {
			continue;
		}

		actor->Tick();
		actor->CheckTouching();
	}

	// Now clean everything up that was marked for destruction
	for ( auto &i : destructionQueue ) {
		assert( actorsList.find( i ) != actorsList.end() );
		actorsList.erase( i );
		delete i;
	}
	destructionQueue.clear();
}

void ActorManager::DrawActors() {
	if ( FrontEnd_GetState() == FE_MODE_LOADING ) {
		return;
	}

	Shaders_SetProgramByName( cv_graphics_debug_normals->b_value ? "debug_normals" : "generic_textured_lit" );

	for ( auto const &actor: actorsList ) {
		actor->Draw();
	}

	if ( cv_debug_bounds->b_value ) {
		Shaders_SetProgramByName( "generic_untextured" );

		for ( auto const &actor: actorsList ) {
			plDrawBoundingVolume( actor->GetBoundingBox(), PL_COLOUR_GREEN );
		}
	}
}

void ActorManager::DestroyActors() {
	for ( auto &actor: actorsList ) {
		delete actor;
	}

	destructionQueue.clear();
	actorsList.clear();
}

void ActorManager::ActivateActors() {
	for ( auto const &actor: actorsList ) {
		actor->Activate();
	}
}

void ActorManager::DeactivateActors() {
	for ( auto const &actor: actorsList ) {
		actor->Deactivate();
	}
}

void ActorManager::RegisterSpawnManifests() {
	plScanDirectory( "actors", "actor", RegisterActorManifest, false, this );
}

void ActorManager::RegisterActorManifest( const char *path, void *userPtr ) {
	ActorManager *actorManager = static_cast<ActorManager *>( userPtr );

	try {
		JsonReader reader( path );

		// Number of actors in this particular manifest
		unsigned int numActors = reader.GetArrayLength();
		if ( numActors == 0 ) {
			Warning( "Empty actor manifest!\n" );
			return;
		}

		for( unsigned int i = 0; i < numActors; ++i ) {
			reader.EnterChildNode( i );

			ActorSpawnManifest manifest;

			// Ensure it's not already been added!
			manifest.identifier = reader.GetStringProperty( "identifier" );
			const auto &find = actorManager->actorSpawnsRegistry.find( manifest.identifier );
			if( find != actorManager->actorSpawnsRegistry.end() ) {
				Warning( "Actor \"%s\" has already been registered!\n", manifest.identifier.c_str() );
				continue;
			}

			manifest.className = reader.GetStringProperty( "className" );
			manifest.description = reader.GetStringProperty( "description" );

			// Read in all the custom properties
			if ( reader.EnterChildNode( "properties" ) ) {
				std::list< std::string > propertyKeys = reader.GetObjectKeys();
				for ( const auto &property : propertyKeys ) {
					std::string value = reader.GetStringProperty( property );
					manifest.properties.emplace( std::pair< std::string, std::string >( property, value ) );
				}
				reader.LeaveChildNode();
			}

			actorManager->actorSpawnsRegistry.emplace( std::pair< std::string, ActorSpawnManifest >( manifest.identifier, manifest ) );

			reader.LeaveChildNode();
		}
	} catch( const std::exception &exception ) {
		Warning( "Failed to load actor manifest, \"%s\"!\nException: %s\n", exception.what() );
		return;
	}
}

ActorManager::ActorClassRegistration::ActorClassRegistration( const std::string &name, actor_ctor_func ctor_func )
		: name_( name ) {
	ActorManager::actorClassesRegistry[ name ] = ctor_func;
}

ActorManager::ActorClassRegistration::~ActorClassRegistration() {
	ActorManager::actorClassesRegistry.erase( name_ );
}
