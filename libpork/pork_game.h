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

enum PigTeam {
    /* ...original... */
    TEAM_BRITISH,
    TEAM_AMERICAN,
    TEAM_FRENCH,
    TEAM_GERMAN,
    TEAM_RUSSIAN,
    TEAM_JAPAN,
    TEAM_LARD,
    /* ...any new types below... */

    MAX_TEAMS
};

enum PigClass {
    /* ...original... */
    PIG_CLASS_ACE,
    PIG_CLASS_LEGEND,
    PIG_CLASS_MEDIC,
    PIG_CLASS_COMMANDO,
    PIG_CLASS_SPY,
    PIG_CLASS_SNIPER,
    PIG_CLASS_SABOTEUR,
    PIG_CLASS_HEAVY,
    PIG_CLASS_GRUNT,
    /* ...any new types below... */

    MAX_CLASSES
};

typedef struct CampaignManifest {
    char path[PL_SYSTEM_MAX_PATH];  /* path to the campaign manifest */
    char dir[64];                   /* directory under campaigns */

    char name[256];
    char version[16];
    char author[128];

    unsigned int num_supported_langueges;
} CampaignManifest;

typedef struct GameModeSetup {
    char map[32];
    uint mode;          /* passed to the map loader */
    uint num_players;

    bool force_start;   /* kills any current game */
} GameModeSetup;

PL_EXTERN_C

CampaignManifest *GetCurrentCampaign(void);

void RegisterCampaigns(void);
void SetCampaign(const char *dir);

/****************************************************/

void StartGame(const char *map, uint mode, uint8_t num_players, bool force_start);
void EndGame(void);

PL_EXTERN_C_END
