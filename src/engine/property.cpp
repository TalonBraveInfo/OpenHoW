/* OpenHoW
 * Copyright (C) 2017-2019 Daniel Collins <solemnwarning@solemnwarning.net>
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
#include "property.h"

Property::Property(PropertyOwner &po, const std::string &name, unsigned flags):
	name(name),
	flags(flags),
	po_(po),
	is_dirty_(false)
{
	auto x = po_.properties_.insert(std::make_pair(name, this));
	u_assert(x.second); /* Check for name collision */
}

Property::~Property()
{
	po_.properties_.erase(name);
}

void Property::MarkClean()
{
	clean_serialised_ = Serialise();
	is_dirty_ = false;
}

void Property::MarkDirty()
{
	if(!is_dirty_)
	{
		is_dirty_ = true;
		dirty_since_ = g_state.sim_ticks;
	}
}

void Property::ResetToClean()
{
	Deserialise(clean_serialised_);
	MarkClean();
}

unsigned int Property::DirtyTicks() const
{
	if(is_dirty_)
	{
		return g_state.sim_ticks - dirty_since_;
	}
	else{
		return 0;
	}
}

PropertyOwner::PropertyOwner() {}
PropertyOwner::~PropertyOwner() {}

std::string PropertyOwner::SerializePropertiesAsJson() {

}
