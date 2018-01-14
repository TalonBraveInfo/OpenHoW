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
#include "pork_actor.h"

void GetCommand(unsigned int argc, char *argv[]) {
    if(argc < 1) {
        return;
    }

    for(unsigned int i = 1; i < argc; ++i) {

    }
}

void SetCommand(unsigned int argc, char *argv[]) {
    if(argc < 1) {
        return;
    }

    for(unsigned int i = 1; i < argc; ++i) {
        if(pl_strncasecmp("actor", argv[i], 5) == 0) {
            const char *actor_name = argv[++i];
            if(actor_name == NULL || actor_name[0] == '\0') {
                LogWarn("invalid actor name, aborting\n");
                return;
            }

            Actor *actor = GetActor(actor_name);
            if(actor == NULL) {
                LogWarn("unable to find actor %s, aborting\n", actor_name);
                return;
            }

            const char *modifier = argv[++i];
            if(modifier == NULL || modifier[0] == '\0') {
                LogWarn("no modifier provided for actor %s, aborting\n", actor_name);
                return;
            }

            /* todo, add table of string + parm so we don't need to hard-code most of this */
            if(pl_strncasecmp("name", modifier, 4) == 0) {
                const char *name = argv[++i];
                if(name == NULL || name[0] == '\0') {
                    LogWarn("invalid name, aborting\n");
                    return;
                }

                strncpy(actor->name, name, sizeof(actor->name));
                return;
            } else if(pl_strncasecmp("position", modifier, 8) == 0) {

            } else if(pl_strncasecmp("bounds", modifier, 6) == 0) {

            }
            return;
        } else if(pl_strncasecmp("map", argv[i], 3) == 0) {
            /* todo, set map properties */
            return;
        }
    }

    LogWarn("invalid parameters provided for set command\n");
}