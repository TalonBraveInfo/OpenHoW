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
#include <pork/pork.h>

#include "pork_engine.h"
#include "server_actor.h"

unsigned int num_actors = 256;
Actor *g_actors = NULL;

void SVClearActors(void) {
    if(g_actors == NULL) {
        LogDebug("allocating %u server actors\n", num_actors);
        if((g_actors = calloc(num_actors, sizeof(Actor))) == NULL) {
            Error("failed to allocate memory for actors, aborting!\n");
        }

        return;
    }

    LogDebug("clearing %u server actors\n", num_actors);
    memset(g_actors, 0, sizeof(Actor) * num_actors);
}

void SVSimulateActors(void) {
    assert(g_actors != NULL);

    for(Actor *actor = g_actors; actor < g_actors + num_actors; ++actor) {
        if(!actor->is_reserved) {
            continue;
        }
    }
}

/* returns the first actor with that specific name */
Actor *SVGetActor(const char *name) {
    /* slightly ugly, but check if the whole string is numeric */
    if(pl_strisalpha(name) == -1) {
        unsigned long slot = strtoul(name, NULL, 0);
        if(slot > num_actors) {
            LogWarn("slot is greater than the number of available actors, aborting!\n");
            return NULL;
        }

        Actor *actor = &g_actors[slot];
        if(actor == NULL) {
            LogWarn("unallocated actor, aborting!\n");
            return NULL;
        }

        if(!actor->is_reserved) {
            LogWarn("chosen actor is not active, aborting!\n");
            return NULL;
        }

        return actor;
    }

    for(Actor *actor = g_actors; actor < g_actors + num_actors; ++actor) {
        if(!actor->is_reserved) {
            continue;
        }

        if(strncmp(name, actor->name, sizeof(actor->name)) == 0) {
            return actor;
        }
    }

    return NULL;
}

/* Actor declarations are passed from
 * json and used for determining the
 * properties of each actor upon spawn
 * and within the editor itself.
 */
typedef struct ActorDeclaration {
    char class[32];         // class name of the actor, so we know what we're spawning
    char description[256];  // short description of the actor - used for editor

    unsigned int type;  // item, pig, vehicle, etc

    char model_path[PL_SYSTEM_MAX_PATH];

    struct {
        struct ActorDeclaration *component;

        PLVector3 position; // position relative to parent
        float angle;        // angle relative to parent
    } components[512];
} ActorDeclaration;

ActorDeclaration actor_declarations[4096];

void InitActors(void) {

    SVClearActors();

    // todo, load in actor_decl.json (or something), containing an outline of how each actor class functions
}

/*********************************************************************/

Actor *Actor_Spawn(void) {
    for(Actor *actor = g_actors; actor < g_actors + num_actors; ++actor) {
        if(!actor->is_reserved) {
            actor->is_reserved = true;
            return actor;
        }
    }

    /* otherwise, attempt to resize the array to accommodate any additional
     * actors we want to add
     *
     * todo, in future, let's be more graceful with this, but for now we'll crash and burn!
     */
    unsigned int old_num_actors = num_actors;
    num_actors += 512;
    if((g_actors = realloc(g_actors, sizeof(Actor) * num_actors)) == NULL) {
        Error("failed to resize actors array to %u, aborting!\n", num_actors);
    }
    memset(g_actors + old_num_actors, 0, sizeof(Actor) * (num_actors - old_num_actors));

    return Actor_Spawn();
}

void Actor_Destroy(Actor *self) {
    if(self == NULL) {
        LogWarn("attempted to free an invalid actor, we'll probably crash!\n");
        return;
    }

    /* will set 'reserved' to false */
    memset(self, 0, sizeof(Actor));
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
    LogDebug("%s took control of actor %s\n", self->name);

    self->controller = player;
    if(self->callback.EPossess) {
        self->callback.EPossess(self, player);
    }
}

void Actor_Depossess(Actor *self, Player *player) {
    if(self->controller == NULL || self->controller != player) {
        return;
    }

    LogDebug("%s released control of actor %s\n", self->name);

    self->controller = NULL;
    if(self->callback.EDepossess) {
        self->callback.EDepossess(self, player);
    }
}