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

#include <PL/platform_filesystem.h>

#include "engine.h"
#include "language.h"
#include "script/script.h"
#include "script/script_config.h"

/* todo: rewrite to take better advantage of std */

typedef struct LanguageKey {
  char key[64];
  char translation[512]; // todo: UTF-8 pls
} LanguageKey;
LanguageKey *l_cache = nullptr;
unsigned int l_cache_keys = 0;

typedef struct Language {
  char key[3];    /* "eng" */
  char name[64];  /* "English" */ // todo: UTF-8 pls
  char font[32];  /* "whatever" */
} Language;
Language *l_manifest = nullptr;

unsigned int num_languages = 0;
unsigned int num_translations = 0;

void Languages_Initialize(void) {
  char man_path[PL_SYSTEM_MAX_PATH];
  strncpy(man_path, u_find("languages.manifest"), sizeof(man_path));

  const char *var;
  if ((var = plGetCommandLineArgumentValue("-manifest")) != nullptr) {
    strncpy(man_path, var, sizeof(man_path));
  }

  LogDebug("caching language data...\n");

  /* load in the language manifest */

#if 0 // todo: rethink this
  num_languages = Script_GetArrayLength(ctx, "languages");
  l_manifest = u_alloc(num_languages, sizeof(Language), true);
  for(unsigned int i = 0; i < num_languages; ++i) {
      strncpy(l_manifest[i].key, Script_GetArrayObjectString(ctx, "languages", i, "key", ""), sizeof(l_manifest[i].key));
      strncpy(l_manifest[i].name, Script_GetArrayObjectString(ctx, "languages", i, "name", ""), sizeof(l_manifest[i].name)); // todo: UTF-8 PLEASE!!!!!!
      /* uncomment once we're using this...
      strncpy(l_manifest[i].font_path, Script_GetStringProperty("font_path"), sizeof(l_manifest[i].font_path)); */
  }

#if _DEBUG /* debug */
  for(unsigned int i = 0; i < num_languages; ++i) {
      printf("key %s\n", l_manifest[i].key);
      printf("name %s\n", l_manifest[i].name);
      printf("font %s\n", l_manifest[i].font);
  }
  printf("done!\n");
#endif

  Script_DestroyContext(ctx);
#endif
}

void Languages_Clear(void) {
  u_free(l_manifest);
  u_free(l_cache);
}

const char *GetTranslation(const char *key) { // todo: UTF-8 pls
  u_assert(plIsEmptyString(key), "invalid translation key!\n");

  if (l_cache == NULL) {
    return key;
  }

  for (uint i = 0; i < l_cache_keys; ++i) {
    if (strcmp(key, l_cache[i].key) == 0) {
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
