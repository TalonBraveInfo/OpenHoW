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
#include "ModelResource.h"

// TODO: we should be able to query the platform library for this!!
const char *supportedModelFormats[] = { "obj", "vtx", "min", nullptr };

ohw::ModelResource::ModelResource( const std::string &path, bool persist, bool abortOnFail ) : Resource( path, persist ) {
	const char *newPath = u_find2( path.c_str(), supportedModelFormats, abortOnFail );
	if ( newPath == nullptr ) {
		// Couldn't find anything, so just set the fallback and cache
		modelPtr = ohw::Engine::Resource()->GetFallbackModel();
		return;
	}

	modelPtr = plLoadModel( newPath );
	if ( modelPtr == nullptr ) {
		if ( abortOnFail ) {
			Error( "Failed to load model, \"%s\"!\nPL: %s\n", newPath, plGetError() );
		}

		modelPtr = ohw::Engine::Resource()->GetFallbackModel();
		LogWarn( "Failed to load model, \"%s\"!\nPL: %s\n", newPath, plGetError() );
	}
}

ohw::ModelResource::~ModelResource() {
	plDestroyModel( modelPtr );
}
