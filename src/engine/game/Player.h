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

enum class PlayerType {
	LOCAL,
	NETWORKED,
	COMPUTER,
};

class Player {
public:
	Player( PlayerType type );
	~Player();

	unsigned int GetNumChildren() { return myChildActors.size(); }

	void AddChild( Actor *actor );
	void RemoveChild( Actor *actor );

	void PossessCurrentChild();
	void DispossessCurrentChild();

	Actor *GetCurrentChild();

	void CycleChildren( bool forward = true );

	void SetControllerSlot( unsigned int slot ) { myControllerSlot = slot; }
	unsigned int GetControllerSlot() { return myControllerSlot; }

	inline void SetTeam( const PlayerTeam &team ) {
		myTeam = team;
	}
	inline const PlayerTeam *GetTeam() const {
		return &myTeam;
	}

protected:
private:
	unsigned int myControllerSlot{ 0 }; // Controller slot

	PlayerType myType;
	PlayerTeam myTeam;

	std::vector< Actor * > myChildActors;
	unsigned int myCurrentChildActorIndex{ 0 };
};
