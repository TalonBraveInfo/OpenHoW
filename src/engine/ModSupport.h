/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

struct ModManifest {
    char path[PL_SYSTEM_MAX_PATH];  /* path to the campaign manifest */
    char dir[64];                   /* directory under campaigns */

    char name[256];
    char version[16];
    char author[128];

    unsigned int num_supported_languages;
};

PL_EXTERN_C

ModManifest *Mod_GetCurrentCampaign(void);

void Mod_RegisterCampaigns(void);
void Mod_RegisterCampaign(const char *path);
void Mod_SetCampaign(const char *dir);

PL_EXTERN_C_END
