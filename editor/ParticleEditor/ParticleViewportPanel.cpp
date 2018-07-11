/* OpenHoW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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

#include "ParticleViewportPanel.h"

#include "pork_particle.h"

#include "client/client_actor.h"

ParticleViewportPanel::ParticleViewportPanel(wxWindow *parent) : ViewportPanel(parent) {
    SetContextId(PORK_ED_CTX_PARTICLE);

    attachment_ = SpawnClientActor();
    if(attachment_ == nullptr) {
        /* todo: handle throws ... */
    }

    //system_ = CreateParticleSystem();
}

ParticleViewportPanel::~ParticleViewportPanel() {
    DestroyClientActor(attachment_);
}

void ParticleViewportPanel::Draw() {
    if(!IsShown()) {
        return;
    }

    ViewportPanel::Draw();

    double delta = GetTimerInterval();

    PreDrawPork(delta);

    /* todo: draw sky for backdrop? */

//    DrawParticleSystem(system_, delta);
    DrawActor(attachment_, delta);

    PostDrawPork();
}
