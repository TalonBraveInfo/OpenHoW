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
#include <PL/platform_filesystem.h>

#include "jsmn/jsmn.h"

#include "pork_engine.h"

void SaveConfig(void) {
    // todo, take current state and save it to config.json
}

// wrapper function thing, simplifying jsmn a little
jsmntok_t *ParseJSON(jsmn_parser *p, unsigned int *num_tokens, const char *path) {
    size_t len = plGetFileSize(PORK_CONFIG);
    if(len == 0) {
        LogInfo("blank config, skipping out on bothering read\n");
        return NULL;
    }

    char buffer[len];
    memset(buffer, 0, sizeof(buffer));

    jsmn_init(p);

    int ret = jsmn_parse(p, buffer, len, NULL, 256);
    if(ret < 0) {
        if(ret == JSMN_ERROR_INVAL) {
            LogWarn("bad token, JSON string is corrupted!\n");
        } else if(ret == JSMN_ERROR_NOMEM) {
            LogWarn("not enough tokens, JSON string is too large!\n");
        } else if(ret == JSMN_ERROR_PART) {
            LogWarn("JSON string is too short, expecting more JSON data!\n");
        }
        return NULL;
    }

    *num_tokens = (unsigned int) ret;
    jsmntok_t *tokens = malloc(*num_tokens * sizeof(jsmntok_t));
    if(tokens == NULL) {
        Error("failed to allocate enough memory for tokens, aborting!\n");
    }

    jsmn_parse(p, buffer, len, tokens, *num_tokens);
    return tokens;
}

void InitConfig(void) {
    jsmn_parser p;
    unsigned int num_tokens = 0;
    jsmntok_t *tokens = ParseJSON(&p, &num_tokens, PORK_CONFIG);
    for(unsigned int i = 0; i < num_tokens; ++i) {
        // todo
    }
}