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

CLActor *client_actors = NULL;
unsigned int num_cl_actors = 256;

/**
 * clears all of the current actors and is also used
 * for initializing the actor slots in memory.
 */
void CLClearActors(void) {
    if(client_actors == NULL) {
        client_actors = pork_alloc(num_cl_actors, sizeof(CLActor), true);
    }

    LogDebug("clearing %u client actors\n", num_cl_actors);
    memset(client_actors, 0, sizeof(CLActor) * num_cl_actors);
}

/**
 * initializes client-side actors.
 */
void CLInitActors(void) {
    CLClearActors();
}

/*********************************************************************/

/**
 * spawn client-side actor.
 *
 * @return pointer to client-side actor.
 */
CLActor *CLSpawnActor(void) {
    for(CLActor *actor = client_actors; actor < client_actors + num_cl_actors; ++actor) {
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
    unsigned int old_num_actors = num_cl_actors;
    num_cl_actors += 512;
    if((client_actors = realloc(client_actors, sizeof(CLActor) * num_cl_actors)) == NULL) {
        Error("failed to resize actors array to %u, aborting!\n", num_cl_actors);
    }
    memset(client_actors + old_num_actors, 0, sizeof(CLActor) * (num_cl_actors - old_num_actors));

    return CLSpawnActor();
}

/**
 * free up the actor slot - allowing it to be used
 * by a new actor later.
 *
 * @param actor the client-side actor to destroy.
 */
void CLDestroyActor(CLActor *actor) {
    if(actor == NULL) {
        LogWarn("attempted to free an invalid actor, we'll probably crash!\n");
        return;
    }

    /* will set 'reserved' to false */
    memset(actor, 0, sizeof(CLActor));
}

/*********************************************************************/

/**
 * draws any client-side actors that are currenty reserved and visible.
 *
 * @param delta
 */
void DrawActors(double delta) {
    pork_assert(client_actors != NULL);

    for(CLActor *actor = client_actors; actor < client_actors + num_cl_actors; ++actor) {
        if(!actor->is_reserved || !actor->is_visible) {
            continue;
        }

        if(actor->model != NULL) {
//            Model_ApplyTeamTextures(actor->model, actor->team);
            plDrawModel(actor->model);
            if(cv_debug_skeleton->b_value) {
                plDrawModelSkeleton(actor->model);
            }
        }
    }
}
