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

#pragma once

///////////////////////////////////////////////////
// Actor

typedef struct Actor {
    char name[32]; //aka, class... used as simple ident

    bool is_reserved; // if the actor is free, we can use it as a new actor

    struct Player *controller; // which player is currently controlling us?

    PLVector3 position;
    PLVector3 angles;
    PLVector3 bounds;   // extend from origin

    uint16_t flags;

    struct { // special logic
        uint16_t type;
        uint8_t group;
        uint8_t parameters[19];
        PLVector3 target_position;

        uint16_t spawn_delay; // counted in turns
    } logic;

    // allow us to implement custom interfaces through PLModel?
    // would let us add support for Hogs of War's model format while
    // also leaving leg room for supporting other formats! Would be neato :)
    PLModel *model;

    struct {
        uint32_t item_id;
        int16_t quantity; // -1 value denotes infinite?
        /* once thrown, should negate quantity if more than 1 (otherwise remove if 0)
         * and then should spawn an actor necessary for the weapon projectile to
         * function as it should - can have a table for this of some description?
         */
    } inventory[MAX_INVENTORY];
    uint8_t current_item; // matched against inventory slot

    struct {
        void(*EPossess)(struct Actor *self, struct Player *controller);
        void(*EDepossess)(struct Actor *self, struct Player *controller);
    } callback;

    uint8_t team;       // red, green, blue etc.
    uint8_t class_num;  // spy, sniper, engineer etc.
    int16_t health;

    // some objects in hogs of war have other crap
    // attached / involved in it's behaviour - for
    // this we'll just include child/parent pointers
    struct Actor *child;
    struct Actor *parent;
} Actor;

void SVClearActors(void);
void SVSimulateActors(void);

Actor *SVGetActor(const char *name);

Actor *Actor_Spawn(void);
void Actor_Destroy(Actor *self);
void Actor_Possess(Actor *self, Player *player);
void Actor_Depossess(Actor *self, Player *player);