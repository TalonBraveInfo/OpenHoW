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

#pragma once

#define IMPLEMENT_RESOURCE_CLASS( A ) \
PL_INLINE size_t GetMemoryUsage() const override { return sizeof( A ); }

namespace ohw {
	class Resource {
	public:
		explicit Resource( const std::string &path, bool persist = false );
		virtual ~Resource();

		PL_INLINE unsigned int AddReference() { return ( ++numReferences ); }
		PL_INLINE unsigned int GetReferenceCount() { return numReferences; }
		void Release();

		bool CanDestroy();

		PL_INLINE virtual size_t GetMemoryUsage() const = 0;

	private:
		unsigned int numReferences{ 0 };
		bool persist{ false };
	};
}
