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

#include <stdlib.h>

#include <PL/platform_filesystem.h>

#include "engine.h"

/****************************************************/
/* Memory */

unsigned long u_unmangle(void *source, void *destination) {
    /* todo */

    return 0;
}

void *u_alloc(size_t num, size_t size, bool abort_on_fail) {
    void *mem = calloc(num, size);
    if(mem == NULL && abort_on_fail) {
        Error("failed to allocate %u bytes!\n", size * num);
    }
    return mem;
}

/****************************************************/
/* Filesystem */

const char *u_find(const char *path) {
    static char n_path[PL_SYSTEM_MAX_PATH];
    if(!plIsEmptyString(GetCampaignPath())) {
        snprintf(n_path, sizeof(n_path), "%s/campaigns/%s/%s", GetBasePath(), GetCampaignPath(), path);
        if(plFileExists(n_path)) {
            LogDebug("found \"%s\"\n", n_path);
            return n_path;
        }
    }

    snprintf(n_path, sizeof(n_path), "%s/%s", GetBasePath(), path);
    LogDebug("found \"%s\"\n", n_path);
    return n_path;
}

const char *u_scan(const char *path, const char **preference) {
    static char find[PL_SYSTEM_MAX_PATH];
    while(*preference != NULL) {
        snprintf(find, sizeof(find), "%s.%s", path, *preference);
        if(plFileExists(find)) {
            LogDebug("found \"%s\"\n", find);
            return find;
        } preference++;
    }

    LogDebug("failed to find \"%s\"\n", path);
    return "";
}

const char *u_find2(const char *path, const char **preference) {
    static char out[PL_SYSTEM_MAX_PATH];
    memset(out, 0, sizeof(out));

    char base_path[PL_SYSTEM_MAX_PATH];
    if(!plIsEmptyString(GetCampaignPath())) {
        snprintf(base_path, sizeof(base_path), "%s/campaigns/%s/%s", GetBasePath(), GetCampaignPath(), path);
        strncpy(out, u_scan(base_path, preference), sizeof(out));
        if(!plIsEmptyString(out)) {
            LogDebug("found \"%s\"\n", out);
            return out;
        }
    }

    snprintf(base_path, sizeof(base_path), "%s/%s", GetBasePath(), path);
    strncpy(out, u_scan(base_path, preference), sizeof(out));
    if(plIsEmptyString(out)) {
        Error("failed to find \"%s\"!\n", path);
    }

    LogDebug("found \"%s\"\n", out);
    return out;
}

/*
FILE *u_open(const char *path, const char *mode) {
    FILE *fp;

    char n_path[PL_SYSTEM_MAX_PATH];
    if(!plIsEmptyString(GetModPath())) {
        snprintf(n_path, sizeof(n_path), "%s/mods/%s/%s", GetBasePath(), GetModPath(), path);
        if(plFileExists(n_path)) {
            fp = fopen(n_path, mode);
            if(fp != NULL) {
                return fp;
            }

            LogWarn("failed to load file at %s, falling back to base!\n", n_path);
        }
    }

    snprintf(n_path, sizeof(n_path), "%s/%s", GetBasePath(), path);
    if(plFileExists(n_path)) {
        fp = fopen(n_path, mode);
        if(fp != NULL) {
            return fp;
        }
    }

    LogWarn("failed to load file at %s!\n", n_path);
    return NULL;
}
 */
