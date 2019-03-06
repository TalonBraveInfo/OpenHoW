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

class ScriptConfig {
public:
    explicit ScriptConfig(const std::string &path);
    ScriptConfig();
    ~ScriptConfig();

    void ParseBuffer(const char *buf);

    std::string GetStringProperty(const std::string &property, const std::string &def = "");
    int GetIntegerProperty(const std::string &property, int def = 0);
    bool GetBooleanProperty(const std::string &property, bool def = false);

    unsigned int GetArrayLength(const std::string &property);
    std::vector<std::string> GetArrayStrings(const std::string &property);
    std::string GetArrayStringProperty(const std::string &property, unsigned int index);

protected:
private:
    void* ctx_{nullptr};
};
