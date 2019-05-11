/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include "../engine.h"
#include "../input.h"

#include "ActorManager.h"
#include "ModelActor.h"
#include "AnimatedModelActor.h"

class PigActor : public AnimatedModelActor {
public:
    PigActor();
    ~PigActor() override;

    void HandleInput() override;

protected:
private:
    enum {
        EYES_OPEN,
        EYES_CLOSED,
        EYES_SAD,
        EYES_SURPRISED,
        EYES_ANGRY,
    } upper_face_frame_{EYES_OPEN};

    enum {
        MOUTH_OPEN,
        MOUTH_CLOSED,
        MOUTH_SAD,
        MOUTH_SURPRISED,
        MOUTH_ANGRY,
    } lower_face_frame_{MOUTH_OPEN};
};

PigActor::PigActor() = default;
PigActor::~PigActor() = default;

void PigActor::HandleInput() {
    // todo: implement...
    Actor::HandleInput();
}

register_actor(AC_ME, PigActor);    // Ace
register_actor(LE_ME, PigActor);    // Legend
register_actor(ME_ME, PigActor);    // Medic
register_actor(SB_ME, PigActor);    // Commando
register_actor(SP_ME, PigActor);    // Spy
register_actor(SN_ME, PigActor);    // Sniper
register_actor(SA_ME, PigActor);    // Saboteur
register_actor(GR_ME, PigActor);    // Grunt
