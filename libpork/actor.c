/* OpenHOW
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
#include <pork/pork.h>

#include "engine.h"

unsigned int num_actors = 256;
Actor *g_actors = NULL;

void ClearActors(void) {
    // check so we can use this on shutdown...
    if(g_actors == NULL) {
        return;
    }

    print_debug("clearing %d actors\n", num_actors);
    memset(g_actors, 0, sizeof(Actor) * num_actors);
}

void SimulateActors(float delta) {
    assert(g_actors != NULL);

    for(Actor *actor = g_actors; actor < g_actors + num_actors; ++actor) {
        if(!actor->is_reserved) {
            continue;
        }

        // todo
    }
}

void DrawActors(void) {
    assert(g_actors != NULL);

    for(Actor *actor = g_actors; actor < g_actors + num_actors; ++actor) {
        if(!actor->is_reserved || !actor->is_visible) {
            continue;
        }
    }
}

void InitActors(void) {
    g_actors = calloc(num_actors, sizeof(Actor));
    if(g_actors == NULL) {
        print_error("failed to allocate memory for actors, aborting!\n");
    }

    ClearActors();
}

//////////////////////////////////////////////////////////////

Actor *Actor_Spawn(void) {
    for(unsigned int i = 0; i < num_actors; ++i) {
        if(!g_actors[i].is_reserved) {
            Actor *actor = &g_actors[i];
            snprintf(actor->name, sizeof(actor->name), "actor_%d", i);
            actor->is_reserved = true;
            return actor;
        }
    }

    unsigned int old_num_actors = num_actors;
    // todo, update the array size
    // ensure all new actors all nullified...
    memset(g_actors + old_num_actors, 0, sizeof(Actor) * (num_actors - old_num_actors));
    return Actor_Spawn();
}

void Actor_Destroy(Actor *self) {
    if(self == NULL) {
        print_warning("attempted to free an invalid actor, we'll probably crash!\n");
        return;
    }

    memset(self, 0, sizeof(Actor)); // will set 'reserved' to false
}

void Actor_Possess(Actor *self, Player *player) {
    if(self->controller != NULL && self->controller == player) {
        return;
    }

/* if it's a controllable type and a controller has been
 * set, then once that frame begins, we'll skip any AI sim
 * because it's been occupied by a player - otherwise it
 * will operate on it's own accord.
 */
    print_debug("%s took control of actor %s\n", self->name);

    self->controller = player;
    if(self->callback.EPossess) {
        self->callback.EPossess(self, player);
    }
}

void Actor_Depossess(Actor *self, Player *player) {
    if(self->controller == NULL || self->controller != player) {
        return;
    }

    print_debug("%s released control of actor %s\n", self->name);

    self->controller = NULL;
    if(self->callback.EDepossess) {
        self->callback.EDepossess(self, player);
    }
}