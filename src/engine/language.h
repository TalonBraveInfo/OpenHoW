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

class LanguageManager {
 public:
  static LanguageManager* GetInstance() {
    if(language_manager_ == nullptr) {
      language_manager_ = new LanguageManager();
    }
    return language_manager_;
  }

  static void DestroyInstance() {
    delete language_manager_;
  }

  void SetLanguage(const char* key);
  const char* GetTranslation(const char* key);

  static void SetLanguageCallback(const PLConsoleVariable* var);

 protected:
 private:
  static LanguageManager* language_manager_;

  LanguageManager();
  ~LanguageManager();

  struct Key {
    std::string translation;
  };

  struct Index {
    std::string                 key;
    std::string                 name;
    std::string                 font;
    std::map<std::string, Key>  keys;
  };

  std::map<std::string, Index> languages_;
  Index* current_language{ nullptr };
};

extern LanguageManager g_translation;
