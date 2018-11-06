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

#include "pork_engine.h"
#include "pork_language.h"

#include "script/script.h"

typedef struct LanguageKey {
    char key[64];
    char translation[512]; // todo: UTF-8 pls
} LanguageKey;
LanguageKey *l_cache = NULL;
unsigned int l_cache_keys = 0;

typedef struct Language {
    char key[3];    /* "eng" */
    char name[64];  /* "English" */ // todo: UTF-8 pls
    char font[32];  /* "whatever" */
} Language;
Language *l_manifest = NULL;

unsigned int num_languages = 0;
unsigned int num_translations = 0;

void RegisterLanguages(void) {
    char man_path[PL_SYSTEM_MAX_PATH];
    strncpy(man_path, pork_find("languages.manifest"), sizeof(man_path));

    const char *var;
    if((var = plGetCommandLineArgumentValue("-manifest")) != NULL) {
        strncpy(man_path, var, sizeof(man_path));
    }

    LogDebug("caching language data...\n");

    /* load in the language manifest and store it in a buffer */
    FILE *fp = fopen(man_path, "r");
    if(fp == NULL) Error("failed to load \"%s\"!\n", man_path);
    size_t length = plGetFileSize(man_path);
    char buf[length + 1];
    if(fread(buf, sizeof(char), length, fp) != length) Error("failed to read entirety of language manifest!\n");
    fclose(fp);
    buf[length] = '\0';

    /* now read it in */

    ScriptContext *ctx = Script_CreateContext();
    Script_ParseBuffer(ctx, buf);

    num_languages = Script_GetArrayLength(ctx, "languages");
    l_manifest = pork_alloc(num_languages, sizeof(Language), true);
    for(unsigned int i = 0; i < num_languages; ++i) {
        strncpy(l_manifest[i].key, Script_GetArrayObjectString(ctx, "languages", i, "key", ""), sizeof(l_manifest[i].key));
        strncpy(l_manifest[i].name, Script_GetArrayObjectString(ctx, "languages", i, "name", ""), sizeof(l_manifest[i].name)); // todo: UTF-8 PLEASE!!!!!!
        /* uncomment once we're using this...
        strncpy(l_manifest[i].font_path, Script_GetStringProperty("font_path"), sizeof(l_manifest[i].font_path)); */
    }

#if 0 /* debug */
    for(unsigned int i = 0; i < num_languages; ++i) {
        printf("key %s\n", l_manifest[i].key);
        printf("name %s\n", l_manifest[i].name);
        printf("path %s\n", l_manifest[i].path);
    }
    printf("done!\n");
#endif

    Script_DestroyContext(ctx);
}

void ClearLanguages(void) {
    pork_free(l_manifest);
    pork_free(l_cache);
}

const char *GetTranslation(const char *key) { // todo: UTF-8 pls
    pork_assert(plIsEmptyString(key), "invalid translation key!\n");

    if(l_cache == NULL) {
        return key;
    }

    for(uint i = 0; i < l_cache_keys; ++i) {
        if(strcmp(key, l_cache[i].key) == 0) {
            return l_cache[i].translation;
        }
    }

    return key;
}

void SetLanguageCallback(const PLConsoleVariable *var) {
#if 0 // todo
    pork_assert(var->s_value != NULL, "invalid language key provided!\n");

    pork_free(l_cache);

    // todo: load in the .language data ...

    l_cache = malloc(sizeof(l_cache));
    memset(l_cache, 0, sizeof(LanguageKey) * 2048); // todo: get the number of keys (done during load?) ...

    // todo: how about the rest? ...
#endif
}
