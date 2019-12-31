/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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

class ScriptConfig {
 public:
  explicit ScriptConfig(const std::string& path);
  ScriptConfig();
  ~ScriptConfig();

  void ParseBuffer(const char* buf);

  void EnterChildNode(const std::string& property);
  void EnterChildNode(unsigned int index);
  void LeaveChildNode();

  std::string GetStringProperty(const std::string& property, const std::string& def = "", bool silent = true);
  int GetIntegerProperty(const std::string& property, int def = 0, bool silent = true);
  bool GetBooleanProperty(const std::string& property, bool def = false, bool silent = true);
  float GetFloatProperty(const std::string& property, float def = 0.0f, bool silent = true);
  PLColour GetColourProperty(const std::string& property, PLColour def = PLColour(0, 0, 0), bool silent = true);
  PLVector3 GetVector3Property(const std::string& property, PLVector3 def = PLVector3(0, 0, 0), bool silent = true);

  std::list<std::string> GetObjectKeys();

  unsigned int GetArrayLength(const std::string& property = "");
  std::vector<std::string> GetArrayStrings(const std::string& property);
  std::string GetArrayStringProperty(const std::string& property, unsigned int index);

 protected:
 private:
  void* ctx_{nullptr};
};
