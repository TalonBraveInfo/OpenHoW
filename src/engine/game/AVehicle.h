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

#include "AModel.h"

class AVehicle : public AModel {
	IMPLEMENT_ACTOR( AVehicle, AModel )

public:
	AVehicle();
	~AVehicle();

	void Tick() override;

	virtual bool EnterVehicle( Actor *occupant );
	virtual void ExitVehicle( Actor *occupant );

	bool IsOccupied() const;

	Actor *GetOccupant( unsigned int slot );

	virtual inline unsigned int GetMaxOccupants() const { return 1;	}
	virtual inline const char *GetEngineIdleSound() const {	return "audio/en_tank.wav";	}

	void Deserialize( const ActorSpawn &spawn ) override;

protected:
private:
	std::vector< Actor * > myOccupants;

	AudioSource *audioSource{ nullptr };
};