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

#include "pork_engine.h"
#include "pork_model.h"

#include "client_actor.h"
#include "client_frontend.h"

/* todo: all of this is going to be torn out, so don't get too deep into it */

ClientActor *client_actors = NULL;
unsigned int num_client_actors = 256;

/**
 * clears all of the current actors and is also used
 * for initializing the actor slots in memory.
 */
void ClearClientActors(void) {
    if(client_actors == NULL) {
        client_actors = pork_alloc(num_client_actors, sizeof(ClientActor), true);
    }

    LogDebug("clearing %u client actors\n", num_client_actors);
    memset(client_actors, 0, sizeof(ClientActor) * num_client_actors);
}

/**
 * initializes client-side actors.
 */
void InitClientActors(void) {
    ClearClientActors();
}

/*********************************************************************/

/**
 * spawn client-side actor.
 *
 * @return pointer to client-side actor.
 */
ClientActor *SpawnClientActor(void) {
    for(ClientActor *actor = client_actors; actor < client_actors + num_client_actors; ++actor) {
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
    unsigned int old_num_actors = num_client_actors;
    num_client_actors += 512;
    if((client_actors = realloc(client_actors, sizeof(ClientActor) * num_client_actors)) == NULL) {
        Error("failed to resize client actors array to %u, aborting!\n", num_client_actors);
    }
    memset(client_actors + old_num_actors, 0, sizeof(ClientActor) * (num_client_actors - old_num_actors));

    return SpawnClientActor();
}

/**
 * free up the actor slot - allowing it to be used
 * by a new actor later.
 *
 * @param actor the client-side actor to destroy.
 */
void DestroyClientActor(ClientActor *actor) {
    if(actor == NULL) {
        LogWarn("attempted to free an invalid actor, we'll probably crash!\n");
        return;
    }

    /* will set 'reserved' to false */
    memset(actor, 0, sizeof(ClientActor));
}

/*********************************************************************/

void DrawActor(ClientActor *actor, double delta) {
    pork_assert(client_actors != NULL);

    if(!actor->is_reserved || !actor->is_visible) {
        return;
    }

    if(actor->model != NULL) {
//            Model_ApplyTeamTextures(actor->model, actor->team);
        plDrawModel(actor->model);
        if(cv_debug_skeleton->b_value) {
            plDrawModelSkeleton(actor->model);
        }
    }
}

/**
 * draws any client-side actors that are currenty reserved and visible.
 *
 * @param delta
 */
void DrawActors(double delta) {
    pork_assert(client_actors != NULL);

    if(GetFrontendState() == FE_MODE_LOADING) {
        return;
    }

    for(ClientActor *actor = client_actors; actor < client_actors + num_client_actors; ++actor) {
        DrawActor(actor, delta);
    }
}
