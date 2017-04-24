/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include "platform_graphics.h"
#include "platform_filesystem.h"

using namespace pl::graphics;

#define SHADER_INVALID_TYPE ((PLuint)0 - 1)

unsigned int _plTranslateShaderType(ShaderType type) {
#if defined(PL_MODE_OPENGL)
    switch(type) {
        case SHADER_VERTEX:      return GL_VERTEX_SHADER;
        case SHADER_FRAGMENT:    return GL_FRAGMENT_SHADER;
#if defined(PL_MODE_OPENGL_CORE)
        case SHADER_GEOMETRY:    return GL_GEOMETRY_SHADER;
#endif
#ifndef __APPLE__
        case SHADER_COMPUTE:     return GL_COMPUTE_SHADER;
#endif
        default: return SHADER_INVALID_TYPE;
    }
#else
    return type;
#endif
}

unsigned int _plTranslateUniformType(ShaderUniformType type) {
#if defined(PL_MODE_OPENGL)
    switch(type) {
        case UNIFORM_DOUBLE:    return GL_DOUBLE;
        case UNIFORM_BOOL:      return GL_BOOL;
        case UNIFORM_FLOAT:     return GL_FLOAT;
        case UNIFORM_INT:       return GL_INT;
        case UNIFORM_UINT:      return GL_UNSIGNED_INT;

        case UNIFORM_SAMPLER1D:         return GL_SAMPLER_1D;
        case UNIFORM_SAMPLER1DSHADOW:   return GL_SAMPLER_1D_SHADOW;
        case UNIFORM_SAMPLER2D:         return GL_SAMPLER_2D;
        case UNIFORM_SAMPLER2DSHADOW:   return GL_SAMPLER_2D_SHADOW;
        case UNIFORM_SAMPLER3D:         return GL_SAMPLER_3D;
        case UNIFORM_SAMPLERCUBE:       return GL_SAMPLER_CUBE;

        case UNIFORM_VEC2:  return GL_FLOAT_VEC2;
        case UNIFORM_VEC3:  return GL_FLOAT_VEC3;
        case UNIFORM_VEC4:  return GL_FLOAT_VEC4;

        case UNIFORM_MAT3:  return GL_FLOAT_MAT3;

        default:    return GL_ZERO;
    }
#else
    return type;
#endif
}

#if defined(PL_MODE_OPENGL)
ShaderUniformType _plTranslateGLUniformType(unsigned int type) {
    switch(type) {
        case GL_DOUBLE:         return UNIFORM_DOUBLE;
        case GL_BOOL:           return UNIFORM_BOOL;
        case GL_FLOAT:          return UNIFORM_FLOAT;
        case GL_INT:            return UNIFORM_INT;
        case GL_UNSIGNED_INT:   return UNIFORM_UINT;

        case GL_SAMPLER_1D:         return UNIFORM_SAMPLER1D;
        case GL_SAMPLER_1D_SHADOW:  return UNIFORM_SAMPLER1DSHADOW;
        case GL_SAMPLER_2D:         return UNIFORM_SAMPLER2D;
        case GL_SAMPLER_2D_SHADOW:  return UNIFORM_SAMPLER2DSHADOW;
        case GL_SAMPLER_3D:         return UNIFORM_SAMPLER3D;
        case GL_SAMPLER_CUBE:       return UNIFORM_SAMPLERCUBE;

        case GL_FLOAT_VEC2: return UNIFORM_VEC2;
        case GL_FLOAT_VEC3: return UNIFORM_VEC3;
        case GL_FLOAT_VEC4: return UNIFORM_VEC4;

        case GL_FLOAT_MAT3: return UNIFORM_MAT3;

        default: return UNIFORM_END;
    }
}
#endif

/*===========================
	SHADER
===========================*/

Shader::Shader(ShaderType type, std::string path) : type_(type) {
    PLuint ntype = _plTranslateShaderType(type_);
    if(ntype == SHADER_INVALID_TYPE) {
        throw std::runtime_error("invalid shader type received!");
    }

#if defined(PL_MODE_OPENGL)
    id_ = glCreateShader(_plTranslateShaderType(type_));
    if(id_ == 0) {
        throw std::runtime_error("failed to create shader");
    } else if(!path.empty() && (LoadFile(path) != PL_RESULT_SUCCESS)) {
        throw std::runtime_error("failed to load shader");
    }
#endif
}

Shader::~Shader() {
#if defined(PL_MODE_OPENGL)
    if(program_) {
        glDetachShader(program_->GetInstance(), id_);
    }

    glDeleteShader(id_);
#endif
}

PLresult Shader::LoadFile(std::string path) {
    // Ensure we use the correct path and shader.
    std::string full_path;
    switch(type_) {
        case SHADER_FRAGMENT:
            full_path = path + "_fragment.shader";
            break;
        case SHADER_VERTEX:
            full_path = path + "_vertex.shader";
            break;
        default:    return PL_RESULT_FILETYPE;
    }

    std::ifstream file(full_path, std::ios::in);
    if(!file.is_open()) {
        return PL_RESULT_FILEREAD;
    }
    file.seekg(0, file.end);
    long size = file.tellg();
    file.seekg(0, file.beg);
    char *buf = new char[size];
    file.read(buf, size);

    const char *full_source[] = {
#if 0
#if defined(PL_MODE_OPENGL)
        //"#version 110\n",	// OpenGL 2.0
        "#version 120\n",	// OpenGL 2.1
        //"#version 130\n",	// OpenGL 3.0
        //"#version 140\n",	// OpenGL 3.1
        //"#version 150\n",	// OpenGL 3.2
        //"#version 330\n", // OpenGL 3.3
        //"#version 450\n",	// OpenGL 4.5
#elif defined(PL_MODE_OPENGL_ES)
        "#version 100\n",	// OpenGL ES 2.0
#endif
#endif
        buf
    };
    // todo, introduce pre-processor to catch any custom ones (like include).
    glShaderSource(id_, 2, full_source, NULL);
    glCompileShader(id_);

    delete[] buf;

    int status;
    glGetShaderiv(id_, GL_COMPILE_STATUS, &status);
    if(!status) {
        int length;
        glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &length);
        if(length > 1) {
            char *log = new char[length];
            glGetShaderInfoLog(id_, length, NULL, log);
            plGraphicsLog(log);
            delete[] log;
        }

        return PL_RESULT_SHADERCOMPILE;
    }

    return PL_RESULT_SUCCESS;
}

/*===========================
	SHADER UNIFORM
===========================*/

ShaderUniform::ShaderUniform(ShaderProgram *parent, std::string name, ShaderUniformType type) {
    if(!parent) {
        throw std::runtime_error("invalid shader program");
    } else if(parent->GetInstance() == 0) {
        throw std::runtime_error("invalid shader program instance, might not yet have been created");
    }
    parent_ = parent;

    if(name.empty()) {
        throw std::runtime_error("received invalid uniform name");
    }

    int location = glGetUniformLocation(parent_->GetInstance(), name.c_str());
    if(location == -1) {
        throw std::runtime_error("failed to get uniform location");
    }
    id_ = static_cast<unsigned int>(location);

    type_ = type;
}

void ShaderUniform::Set(double x) {
    if(type_ != UNIFORM_DOUBLE) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL_CORE)
    glUniform1d(id_, x);
#endif
}

void ShaderUniform::Set(float x) {
    if(type_ != UNIFORM_FLOAT) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUniform1f(id_, x);
#endif
}

void ShaderUniform::Set(float x, float y) {
    if(type_ != UNIFORM_VEC2) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUniform2f(id_, x, y);
#endif
}

void ShaderUniform::Set(float x, float y, float z) {
    if(type_ != UNIFORM_VEC3) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUniform3f(id_, x, y, z);
#endif
}

void ShaderUniform::Set(float x, float y, float z, float w) {
    if(type_ != UNIFORM_VEC4) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUniform4f(id_, x, y, z, w);
#endif
}

void ShaderUniform::Set(float *x, unsigned int size) {
    if(size < 1 || size > 4) {
        throw std::out_of_range("invalid size");
    }

    switch(size) {
        default:
        case 1: {
            Set(x[0]);
            break;
        }
        case 2: {
            Set(x[0], x[1]);
            break;
        }
        case 3: {
            Set(x[0], x[1], x[2]);
            break;
        }
        case 4: {
            Set(x[0], x[1], x[2], x[3]);
            break;
        }
    }
}

void ShaderUniform::Set(int x) {
    if(type_ != UNIFORM_INT) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUniform1i(id_, x);
#endif
}

void ShaderUniform::Set(int x, int y) {
    if(type_ != UNIFORM_VEC2) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUniform2i(id_, x, y);
#endif
}

void ShaderUniform::Set(int x, int y, int z) {
    if(type_ != UNIFORM_VEC3) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUniform3i(id_, x, y, z);
#endif
}

void ShaderUniform::Set(unsigned int x) {
    if(type_ != UNIFORM_UINT) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL_CORE)
    glUniform1ui(id_, x);
#endif
}

void ShaderUniform::Set(PLVector2D x) {
    if(type_ != UNIFORM_VEC2) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUniform2f(id_, x.x, x.y);
#endif
}

void ShaderUniform::Set(PLVector3D x) {
    if(type_ != UNIFORM_VEC3) {
        plGraphicsLog("Invalid shader uniform type! (%i)", type_);
        return;
    }

#if defined(PL_MODE_OPENGL)
    glUniform3f(id_, x.x, x.y, x.z);
#endif
}

/*===========================
	SHADER ATTRIBUTE
===========================*/

ShaderAttribute::ShaderAttribute(ShaderProgram *parent, std::string name) {
    if(!parent) {
        throw std::runtime_error("invalid shader program");
    } else if(parent->GetInstance() == 0) {
        throw std::runtime_error("invalid shader program instance, might not yet have been created");
    }
    parent_ = parent;

    if(name.empty()) { // warning?
        throw std::runtime_error("received invalid attribute name");
    }

    int location = glGetAttribLocation(parent_->GetInstance(), name.c_str());
    if(location == -1) { // warning?
        throw std::runtime_error("failed to get attribute location");
    }
    id_ = static_cast<unsigned int>(location);
}

void ShaderAttribute::Set(float x) {

}

void ShaderAttribute::Set(float x, float y) {

}

void ShaderAttribute::Set(float x, float y, float z) {

}

void ShaderAttribute::Set(float x, float y, float z, float w) {

}

void ShaderAttribute::Set(int x) {

}

void ShaderAttribute::Set(int x, int y) {

}

void ShaderAttribute::Set(int x, int y, int z) {

}

/*===========================
	SHADER PROGRAM
===========================*/

ShaderProgram::ShaderProgram() {
    int id = glCreateProgram();
    if(id == 0) {
        throw std::runtime_error("failed to create shader program");
    }

    id_ = static_cast<unsigned int>(id);
}

ShaderProgram::~ShaderProgram() {
    if(id_ == 0) {
        return;
    }

    glDeleteProgram(id_);
}

void ShaderProgram::Enable() {
    plEnableShaderProgram(id_);
}

void ShaderProgram::Disable() {
    plDisableShaderProgram(id_);
}

void ShaderProgram::RegisterUniform(std::string name, ShaderUniformType type) {
    if(name.empty()) {
        plGraphicsLog("Attempted to register a uniform with an invalid name!\n");
        return;
    }

    // Ensure it's not registered already.
    ShaderUniform *euni = GetUniform(name);
    if(euni) {
        return;
    }

    uniforms.emplace(name, ShaderUniform(this, name, type));
}

void ShaderProgram::RegisterUniforms() {
    int uniforms;
    glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &uniforms);
    for(int i = 0; i < uniforms; ++i) {

        unsigned int type = 0;
        int size = 0, length = 0;
        char name[256] = { 0 };
        glGetActiveUniform(id_, unsigned(i), sizeof(name) - 1, &length, &size, &type, name);
        name[length] = 0;

        RegisterUniform(name, _plTranslateGLUniformType(type));
    }
}

void ShaderProgram::RegisterAttribute(std::string name) {
    if(name.empty()) {
        plGraphicsLog("Attempted to register an attribute with an invalid name!\n");
        return;
    }

    // Ensure it's not registered already.
    ShaderAttribute *eati = GetAttribute(name);
    if(eati) {
        return;
    }

    attributes.emplace(name, ShaderAttribute(this, name));
}

void ShaderProgram::AttachShader(Shader *shader) {
    if(!shader) {
        plGraphicsLog("Attempted to attach an invalid shader!\n");
        return;
    } else if(shader->GetInstance() == 0) {
        plGraphicsLog("Attempted to attach a shader with an invalid instance!\n");
        return;
    }

    glAttachShader(id_, shader->GetInstance());

    shaders.push_back(shader);
}

void ShaderProgram::LoadShaders(std::string vertex, std::string fragment) {
    if(!plFileExists(vertex.c_str()) || !plFileExists(fragment.c_str())) {
        plGraphicsLog("Invalid shader path! (%s / %s)\n", fragment.c_str(), vertex.c_str());
        return;
    }

    AttachShader(new Shader(SHADER_FRAGMENT, fragment));
    AttachShader(new Shader(SHADER_VERTEX, vertex));
}
