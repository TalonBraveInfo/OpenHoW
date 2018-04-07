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
#pragma once

#ifdef __cplusplus

class ShaderStage {
public:
    ShaderStage(unsigned int type);
    virtual ~ShaderStage();

    bool Load(const char *path);

    unsigned int GetInstance() { return gl_id_; }
    unsigned int GetType() { return gl_type_; }

protected:
private:
    unsigned int gl_id_{0};
    unsigned int gl_type_{0};
};

class ShaderProgram {
public:
    ShaderProgram(const char *name);
    virtual ~ShaderProgram();

    virtual void Initialize() = 0;

    void RegisterStage(ShaderStage *stage);
    void RegisterStage(const char *path, unsigned int type);

    virtual void RegisterStages();
    virtual void RegisterAttributes();

    const char *GetName() { return &name_[0]; }

    void Enable();
    void Disable();

protected:
private:
    std::vector<ShaderStage*> stages_;

    std::unordered_map<std::string, unsigned int> attributes_;
    std::unordered_map<std::string, unsigned int> uniforms_;

    bool is_linked_;

    unsigned int gl_id_{0};

    char name_[32];
};

void InitShaders();
void ShutdownShaders();

void DeleteShaderProgram(const char *name);

#endif
