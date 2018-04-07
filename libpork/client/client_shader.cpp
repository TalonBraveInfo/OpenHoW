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
#include <GL/glew.h>
#include "pork_engine.h"
#include "client_shader.h"

#if 0 /* legacy implementation using plgraphics foundation */
PLShaderProgram *shader_default = NULL;

PLShaderProgram *LoadShaderProgram(const char *name) {
    if(plIsEmptyString(name)) {
        Error("invalid name for shader program, aborting!\n");
    }

    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "%sshaders/%s.vert", g_state.base_path, name);
    PLShaderStage *vertex = plLoadShaderStage(path, PL_SHADER_TYPE_VERTEX);
    if(vertex == NULL) {
        Error("failed to load %s, aborting!\n%s", path, plGetError());
    }

    snprintf(path, sizeof(path), "%sshaders/%s.frag", g_state.base_path, name);
    PLShaderStage *fragment = plLoadShaderStage(path, PL_SHADER_TYPE_FRAGMENT);
    if(fragment == NULL) {
        Error("failed to load %s, aborting!\n%s", path, plGetError());
    }

    PLShaderProgram *program = plCreateShaderProgram();
    if(program == NULL) {
        Error("failed to create shader program, %s, aborting!\n%s", name, plGetError());
    }

    plAttachShaderStage(program, vertex);
    plAttachShaderStage(program, fragment);
    plLinkShaderProgram(program);

    return program;
}

void InitShaders(void) {
    shader_default = LoadShaderProgram("default");

    /* enable the default shader program */
    plSetShaderProgram(shader_default);
}

void ShutdownShaders(void) {
    plDeleteShaderProgram(shader_default, true);
}

#else /* new implementation */

ShaderProgram::ShaderProgram(const char *name) {
    pork_assert(name != nullptr && name[0] != '\0');
    snprintf(name_, sizeof(name_), "%s", name);

    gl_id_ = glCreateProgram();
    if(gl_id_ == 0) {
        Error("failed to generate shader program for \"%s\"!\n", name);
    }
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(gl_id_);
}

void ShaderProgram::RegisterStage(ShaderStage *stage) {
    if(stage == nullptr) {
        Error("attempted to register an invalid shader stage with \"%s\", aborting!\n", name_);
    }

    glAttachShader(gl_id_, stage->GetInstance());
    stages_.push_back(stage);
}

void ShaderProgram::RegisterStage(const char *path, unsigned int type) {
    ShaderStage *stage = new ShaderStage(type);
    if(!stage->Load(path)) {
        Error("failed to load shader stage for \"%s\", aborting!\n", name_);
    }

    RegisterStage(stage);
}

void ShaderProgram::Enable() {
    if(!is_linked_) {
        LogWarn("attempted to enable shader program \"%s\" before linking it!\n", name_);
        return;
    }

    glUseProgram(gl_id_);
}

void ShaderProgram::Disable() {
    glUseProgram(0);
}

/*************************************************/
/** Shader API                                  **/

std::unordered_map<std::string, ShaderProgram*> programs;

void InitShaders() {
    LogInfo("initializing shader sub-system...\n");
}

void ShutdownShaders() {
    for(auto program = programs.begin(); program != programs.end(); ++program) {
        delete program->second;
    }

    programs.clear();
}

/**
 * register the shader program into our list.
 *
 * @param program
 * @param name
 */
void RegisterShaderProgram(ShaderProgram *program, const char *name) {
    if(name == NULL || name[0] == '\0') {
        Error("invalid name for shader program, aborting!\n");
    }

    /* todo: do we really want to do this here!? */
    program->Enable();
    program->Initialize();
    program->Disable();

    programs.emplace(name, program);
}

ShaderProgram *GetShaderProgram(const char *name) {
    auto program = programs.find(name);
    if(program != programs.end()) {
        return program->second;
    }

    LogWarn("failed to find shader program \"%s\"!\n", name);
    return nullptr;
}

void DeleteShaderProgram(const char *name) {
    auto program = programs.find(name);
    if(program != programs.end()) {
        delete program->second;
        programs.erase(program);
        return;
    }

    LogWarn("failed to find shader program \"%s\" for deletion!\n", name);
}

void DeleteShaderProgram(ShaderProgram *program) {
    if(program == nullptr) {
        LogDebug("attempted to delete an already null shader program...\n");
        return;
    }

    DeleteShaderProgram(program->GetName());
}

#endif