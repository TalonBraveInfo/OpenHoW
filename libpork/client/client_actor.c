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
#include "client_actor.h"

CLActor *client_actors = NULL;

void DrawActors(double delta) {
#if 0
    assert(g_actors != NULL);

    for(Actor *actor = g_actors; actor < g_actors + num_actors; ++actor) {
        if(!actor->is_reserved || !actor->is_visible) {
            continue;
        }

        if(actor->model != NULL) {
            plDrawModel(actor->model);
            if(cv_debug_skeleton->b_value) {
                plDrawModelSkeleton(actor->model);
            }
        }
    }
#else
#endif
}
