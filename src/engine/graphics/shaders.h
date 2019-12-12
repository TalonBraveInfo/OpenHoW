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

typedef struct PLShaderProgram PLShaderProgram;

class hwShaderProgram {
 public:
  hwShaderProgram(const std::string& vertPath, const std::string& fragPath);
  ~hwShaderProgram();

  void Rebuild();

  void Enable();
  void Disable();

  // This is awful, but we need it for passing shader programs into
  // the platform library. Urgh...
  PLShaderProgram* GetInternalProgram() const { return shaderProgram; }

 private:
  void RegisterShaderStage(const char* path, PLShaderType type);

  std::string vertPath;
  std::string fragPath;

  PLShaderProgram* shaderProgram{ nullptr };
};

hwShaderProgram* Shaders_GetProgram(const std::string& name);

void Shaders_SetProgramByName(const std::string& name);
void Shaders_SetProgram(hwShaderProgram* shaderProgram);

void Shaders_Initialize();
void Shaders_Shutdown();
