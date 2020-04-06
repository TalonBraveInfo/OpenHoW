/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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
#include "actor_animated_model.h"
#include "actor_weapon.h"
#include "weapon_parachute.h"
#include "../inventory.h"

class APig : public AAnimatedModel, public InventoryManager {
	IMPLEMENT_ACTOR( APig, AAnimatedModel )

public:
	APig();
	~APig() override;

	void HandleInput() override;
	void Tick() override;

	void SetClass( unsigned int pclass );
	unsigned int GetClass() { return class_; }

	void SetPersonality( unsigned int personality );
	unsigned int GetPersonality() { return personality_; }

	void SetPlayerOwner( Player *owner );
	const Player *GetPlayerOwner();

	void SetTeam( unsigned int team );
	unsigned int GetTeam() { return team_; }

	bool Possessed( const Player *player ) override;
	void Depossessed( const Player *player ) override;

	bool IsVisibleOnMinimap() override;

	enum class VoiceCategory {
		READY,
		FIRE,
		FIRE2,
		DEATH,
		DEATH2
	};
	void PlayVoiceSample( VoiceCategory category );

	enum class LifeState {
		ALIVE,
		DYING,
		DEAD
	};
	void Killed() override;

	void Deserialize( const ActorSpawn &spawn ) override;

private:
	AWeapon *weapon_{ nullptr };
	AParachuteWeapon *parachute_{ nullptr };

	AudioSource *speech_{ nullptr };

	float aim_pitch_{ 0 };

	unsigned int team_{ 0 };
	unsigned int personality_{ 0 };
	unsigned int class_{ 0 };

	LifeState lifeState{ LifeState::ALIVE };

	enum {
		EYES_OPEN,
		EYES_CLOSED,
		EYES_SAD,
		EYES_SURPRISED,
		EYES_ANGRY,
	} upper_face_frame_{ EYES_OPEN };

	enum {
		MOUTH_OPEN,
		MOUTH_CLOSED,
		MOUTH_SAD,
		MOUTH_SURPRISED,
		MOUTH_ANGRY,
	} lower_face_frame_{ MOUTH_OPEN };
};
