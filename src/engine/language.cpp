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

#include "script/script_config.h"

LanguageManager* LanguageManager::language_manager_;

LanguageManager::LanguageManager() {
  char man_path[PL_SYSTEM_MAX_PATH];
  strncpy(man_path, u_find("languages.manifest"), sizeof(man_path));

  const char *var;
  if ((var = plGetCommandLineArgumentValue("-manifest")) != nullptr) {
    strncpy(man_path, var, sizeof(man_path));
  }

  LogInfo("Loading languages.manifest\n");

  // Load in the languages manifest
  try {
    ScriptConfig manifest(man_path);
    unsigned int num_keys = manifest.GetArrayLength("languages");
    for(unsigned int i = 0; i < num_keys; ++i) {
      Index index;
      manifest.EnterChildNode("languages", i);
      index.name = manifest.GetStringProperty("name");
      LogDebug("Language Name: %s\n", index.name.c_str());
      index.key = manifest.GetStringProperty("key");
      LogDebug("Language Key: %s\n", index.key.c_str());
      //index.font = manifest.GetStringProperty("font");
      //LogDebug("Language Font: %s\n", index.font.c_str());
      manifest.LeaveChildNode();
      languages_.insert(std::pair<std::string, Index>(index.key, index));
    }
  } catch(const std::exception &e) {
    Error("Failed to load languages manifest, \"%s\"!\n", man_path);
  }
}

LanguageManager::~LanguageManager() = default;

const char *LanguageManager::GetTranslation(const char *key) { // todo: UTF-8 pls
  if(key[0] != '$') {
    return key;
  }

  const char* p = ++key;
  if(*p == '\0') {
    LogWarn("Invalid key provided\n");
    return p;
  }

  if(current_language == nullptr) {
    LogWarn("No valid language set\n");
    return p;
  }

  auto i = current_language->keys.find(p);
  if(i == current_language->keys.end()) {
    LogWarn("Failed to find translation key\n");
    return p;
  }

  return i->second.translation.c_str();
}

void LanguageManager::SetLanguage(const char* key) {
  if(current_language != nullptr && current_language->key == key) {
    return;
  }

  auto language = languages_.find(key);
  if(language == languages_.end()) {
    Error("Failed to find specified language, \"%s\"!\n", key);
  }

  current_language = &language->second;

  char filename[64];
  snprintf(filename, sizeof(filename), "languages/%s.language", current_language->key.c_str());

  char path[PL_SYSTEM_MAX_PATH];
  snprintf(path, sizeof(path), "%s", u_find(filename));

  try {
    ScriptConfig manifest(path);
    unsigned int num_keys = manifest.GetArrayLength();
    LogDebug("Keys: %d\n", num_keys);
  } catch(const std::exception &e) {
    Error("Failed to load language manifest, \"%s\"!\n", path);
  }
}

void LanguageManager::SetLanguageCallback(const PLConsoleVariable *var) {
  LanguageManager::GetInstance()->SetLanguage(var->s_value);
}
