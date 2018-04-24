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

#include "client_shader.h"

#if 1

PLShaderProgram *programs[END_SHADER];

PLShaderProgram *LoadShaderProgram(const char *vertex, const char *fragment) {
    if(plIsEmptyString(vertex) || plIsEmptyString(fragment)) {
        Error("invalid name for shader program, aborting!\n");
    }

    PLShaderProgram *program = plCreateShaderProgram();
    if(program == NULL) {
        Error("failed to create shader program, aborting!\n%s", plGetError());
    }

    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "shaders/%s.vert", vertex);
    if(!plRegisterShaderStage(program, pork_find(path), PL_SHADER_TYPE_VERTEX)) {
        Error("failed to register vertex stage, \"%s\", aborting!\n", vertex);
    }

    snprintf(path, sizeof(path), "shaders/%s.frag", fragment);
    if(!plRegisterShaderStage(program, pork_find(path), PL_SHADER_TYPE_FRAGMENT)) {
        Error("failed to register fragment stage, \"%s\", aborting!\n", fragment);
    }

    plLinkShaderProgram(program);

    return program;
}

void InitShaders(void) {
    programs[SHADER_DEFAULT]    = LoadShaderProgram("default", "default");
    //programs[SHADER_VIDEO]      = LoadShaderProgram("video", "video");

    /* enable the default shader program */
    //plSetShaderProgram(programs[SHADER_DEFAULT]);
}

void ShutdownShaders(void) {
    for(unsigned int i = 0; i < END_SHADER; ++i) {
        if(programs[i] == NULL) {
            continue;
        }
        plDeleteShaderProgram(programs[i], true);
    }
}

#else /* new implementation ??? */

/*************************************************/
/** Shader Stage                                **/

ShaderStage::ShaderStage(unsigned int type) {
    gl_id_ = glCreateShader(type);
    if(gl_id_ == 0) {
        Error("failed to generate shader stage!\n");
    }
}

ShaderStage::~ShaderStage() {
    if(parent_ != nullptr) {
        glDetachShader(parent_->GetInstance(), gl_id_);
        parent_ = nullptr;
    }
    glDeleteShader(gl_id_);
    gl_id_ = 0;
}

bool ShaderStage::Load(const char *path) {
    FILE *fp = std::fopen(path, "r");
    if(fp == nullptr) {
        Error("failed to open shader stage at \"%s\", aborting!\n", path);
    }

    size_t length = plGetFileSize(path);
    char buf[length];
    if(std::fread(buf, sizeof(char), length, fp) != length) {
        LogWarn("failed to read in entirety of \"%s\"!\n", path);
    }
    fclose(fp);

    return Compile(buf, length);
}

bool ShaderStage::Compile(const char *buf, size_t length) {
    glShaderSource(gl_id_, 1, &buf, nullptr);

    LogInfo("COMPILING SHADER STAGE...\n");
    glCompileShader(gl_id_);

    int status;
    glGetShaderiv(gl_id_, GL_COMPILE_STATUS, &status);
    if(status == 0) {
        int s_length;
        glGetShaderiv(gl_id_, GL_INFO_LOG_LENGTH, &s_length);
        if(s_length > 1) {
            char log[s_length];
            glGetShaderInfoLog(gl_id_, s_length, nullptr, log);
            LogWarn(" COMPILE ERROR:\n  %s\n", log);
        }
        return false;
    }

    LogInfo(" COMPLETED SUCCESSFULLY!\n");
    return true;
}

/*************************************************/
/** Shader Program                              **/

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

/* default shaders */
#include "shader_base.h"

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
void RegisterShaderProgram(ShaderProgram *program) {
    /* todo: do we really want to do this here!? */
    program->Enable();
    program->Initialize();
    program->Disable();

    programs.emplace(program->GetName(), program);
}

ShaderProgram *GetShaderProgram(const char *name) {
    auto program = programs.find(name);
    if(program != programs.end()) {
        return program->second;
    }

    LogWarn("failed to find shader program \"%s\"!\n", name);
    return nullptr;
}

void DisableShaderProgram() {
    glUseProgram(0);
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