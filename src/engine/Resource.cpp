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
#include "Resource.h"

ohw::Resource::Resource( const std::string &path, bool persist ) : persist( persist ) {
	LogDebug( "Created resource, \"%s\"\n", path.c_str() );
}

void ohw::Resource::Release() {
	if ( numReferences > 0 ) {
		--numReferences;
		return;
	}

#if 0
	if ( !persist && numReferences == 0 ) {
		// Automatically destroy ourselves
		delete this;
	}
#endif
}

bool ohw::Resource::CanDestroy() {
	if ( persist ) {
		return false;
	}

	return ( numReferences == 0 );
}
