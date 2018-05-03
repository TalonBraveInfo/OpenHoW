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

typedef struct Language {
    char key[3];            /* "eng" */
    char name[64];          /* "English" */
    char path[128];         /* "english.json" */
    char font_path[256];    /* "fonts/english/" */
} Language;
Language *l_manifest = NULL;

unsigned int num_languages = 0;
unsigned int num_translations = 0;

void RegisterLanguages(void) {
    if(duk_get_prop_string(jsn_context, -1, "languages") && duk_is_array(jsn_context, -1)) {
        num_languages = (unsigned int)duk_get_length(jsn_context, -1);
        l_manifest = pork_alloc(num_languages, sizeof(Language), true);
        for(unsigned int i = 0; i < num_languages; ++i) {
            duk_get_prop_index(jsn_context, -1, i);
            {
                strncpy(l_manifest[i].key, GetJSONStringProperty("key"), sizeof(l_manifest[i].key));
                strncpy(l_manifest[i].name, GetJSONStringProperty("name"), sizeof(l_manifest[i].name));
                strncpy(l_manifest[i].path, GetJSONStringProperty("path"), sizeof(l_manifest[i].path));
                /* uncomment once we're using this...
                strncpy(l_manifest[i].font_path, GetJSONStringProperty("font_path"), sizeof(l_manifest[i].font_path)); */
            }
            duk_pop(jsn_context);
        }
    } else {
        LogWarn("invalid languages property!\n");
    }
    duk_pop(jsn_context);

#if 0 /* debug */
    for(unsigned int i = 0; i < num_languages; ++i) {
        printf("key %s\n", l_manifest[i].key);
        printf("name %s\n", l_manifest[i].name);
        printf("path %s\n", l_manifest[i].path);
    }
    printf("done!\n");
#endif
}

void ShutdownLanguage(void) {
    pork_free(l_manifest);
    pork_free(l_cache);
}

const char *GetTranslationPath(const char *language) {
    //for(unsigned int i = 0; i < )

    return NULL;
}

void CacheLanguageTranslation(const char *language) {
    if(plIsEmptyString(language)) {
        Error("invalid language selection, aborting!\n");
    }
}

const char *GetTranslation(const char *key) {
    if(plIsEmptyString(key)) {
        LogDebug("invalid key!\n");
        return key;
    }

    if(l_cache != NULL) {
        pork_free(l_cache);
    }

    return key;
}

void SetLanguageCallback(const PLConsoleVariable *var) {
   // CacheLanguageManifest()
}
