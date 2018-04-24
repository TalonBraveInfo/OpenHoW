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
#include "pork_language.h"

#include "script/script.h"

#include <PL/platform_filesystem.h>

/* todo: I know, I know, this needs to support unicode and other fun things... bare with me! */

#define LANGUAGE_PATH   "/languages/"

typedef struct LanguageKey {
    char key[64];
    char translation[512];
} LanguageKey;
LanguageKey *l_cache = NULL;
LanguageKey *l_manifest = NULL;

unsigned int num_languages = 0;
unsigned int num_translations = 0;

void InitLanguage(void) {
    LogDebug("caching language manifest...\n");

    char lang_path[PL_SYSTEM_MAX_PATH];
    snprintf(lang_path, sizeof(lang_path), "%s" LANGUAGE_PATH "language.manifest", GetBasePath());
    FILE *fp = fopen(lang_path, "r");
    if(fp == NULL) {
        LogWarn("failed to load \"" LANGUAGE_PATH "language.manifest\"!\n");
        return;
    }

    size_t length = plGetFileSize(lang_path);
    char buf[length + 1];
    if(fread(buf, sizeof(char), length, fp) != length) {
        LogWarn("failed to read entirety of language manifest!\n");
    }
    fclose(fp);
    buf[length] = '\0';

    ParseJSON(buf);


}

const char *GetTranslationPath(const char *language) {
    //for(unsigned int i = 0; i < )
}

void CacheLanguageTranslation(const char *language) {
    if(plIsEmptyString(language)) {
        Error("invalid language selection, aborting!\n");
    }
}

const char *GetTranslation(const char *key) {
    if(plIsEmptyString(key)) {
        LogDebug("invalid key!\n");
        return "null";
    }

    if(l_cache != NULL) {
        pork_free(l_cache);
    }


}

void SetLanguageCallback(const PLConsoleVariable *var) {
   // CacheLanguageManifest()
}
