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
#include "engine.h"

typedef struct MapEntry {
    const char *name;
    const char *description;
} MapEntry;

// for now these are hard-coded, but
// eventually we'll do this through a
// script instead
MapEntry maps_list[]={
        {"camp", "Boot camp"},
        {"estu", "The War Foundation"},
        {"road", "Routine Patrol"},
        {"trench", "Trench Warfare"},
        {"devi", "Morning Glory!"},
        {"rumble", "Island Invasion"},
        {"zulus", "Under Siege"},
        {"twin", "Communication Breakdown"},
        {"sniper", "The Spying Game"},
        {"mashed", "The Village People"},
        {"guns", "Bangers 'N' Mash"},
        {"liberate", "Saving Private Rind"},
        {"oasis", "Just Deserts"},
        {"fjords", "Glacier Guns"},
        {"eyrie", "Battle Stations"},
        {"bay", "Fortified Swine"},
        {"medix", "Over The Top"},
        {"bridge", "Geneva Convention"},
        {"desval", "I Spy..."},
        {"snake", "Chemical Compound"},
        {"emplace", "Achilles Heal"},
        {"supline", "High And Dry"},
        {"keep", "Assassination"},
        {"tester", "Hero Warship"},
        {"foot", "Hamburger Hill"},
        {"final", "Well, Well, Well!"},
};

typedef struct MapTile {

} MapTile;

typedef struct MapBlock {

} MapBlock;

typedef struct Map {

} Map;

void LoadMap(const char *name) {

}