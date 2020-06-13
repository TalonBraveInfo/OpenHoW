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

#include "actor_model.h"

class AVehicle : public AModel {
	IMPLEMENT_ACTOR( AVehicle, AModel )

public:
	AVehicle();
	~AVehicle();

	virtual void Occupy( Actor *occupant );
	virtual void Unoccupy();
	bool IsOccupied() { return isOccupied_; } //occupant_ == nullptr instead?
	Actor *GetOccupant() { return occupant_; }

protected:
private:
	bool isOccupied_;
	Actor *occupant_;
};