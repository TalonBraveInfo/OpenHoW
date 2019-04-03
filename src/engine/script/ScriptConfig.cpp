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
#include <duktape.h>

#include "../engine.h"
#include "ScriptConfig.h"

#define LogMissingProperty(P)   LogWarn("Failed to get JSON property \"%s\"!\n", (P))
#define LogInvalidArray(P)      LogWarn("Invalid JSON array for property \"%s\"!\n", (P))

ScriptConfig::ScriptConfig(const std::string &path) : ScriptConfig() {
    if(path.empty()) {
        throw std::runtime_error("Empty path for config, aborting!\n");
    }

    size_t sz = plGetFileSize(path.c_str());
    if(sz == 0) {
        throw std::runtime_error("Failed to load file, empty config!\n");
    }

    FILE *fp = fopen(path.c_str(), "rb");
    if(fp == nullptr) {
        throw std::runtime_error("Failed to open config at \"" + path + "\", aborting!\n");
    }

    std::vector<char> buf(sz + 1);
    fread(buf.data(), sizeof(char), sz, fp);
    buf[sz] = '\0';
    fclose(fp);
    ParseBuffer(buf.data());
}

ScriptConfig::ScriptConfig() {
    if((ctx_ = duk_create_heap_default()) == nullptr) {
        throw std::bad_alloc();
    }
}

ScriptConfig::~ScriptConfig() {
    if(ctx_ != nullptr) {
        duk_destroy_heap(static_cast<duk_context *>(ctx_));
    }
}

std::string ScriptConfig::GetStringProperty(const std::string &property, const std::string &def) {
    auto *context = static_cast<duk_context *>(ctx_);

    const char *p = property.c_str();
    if(!duk_get_prop_string(context, -1, p)) {
        duk_pop(context);
        LogMissingProperty(p);
        return def;
    }

    const char *str = duk_safe_to_string(context, -1);
    duk_pop(context);

    return str;
}

int ScriptConfig::GetIntegerProperty(const std::string &property, int def) {
    auto *context = static_cast<duk_context *>(ctx_);

    const char *p = property.c_str();
    if(!duk_get_prop_string(context, -1, p)) {
        duk_pop(context);
        LogMissingProperty(p);
        return def;
    }

    int var = duk_to_int32(context, -1);
    duk_pop(context);

    return var;
}

bool ScriptConfig::GetBooleanProperty(const std::string &property, bool def) {
    auto *context = static_cast<duk_context *>(ctx_);

    const char *p = property.c_str();
    if(!duk_get_prop_string(context, -1, p)) {
        duk_pop(context);
        LogMissingProperty(p);
        return def;
    }

    auto var = static_cast<bool>(duk_to_boolean(context, -1));
    duk_pop(context);

    return var;
}

float ScriptConfig::GetFloatProperty(const std::string &property, float def) {
    auto *context = static_cast<duk_context *>(ctx_);

    const char *p = property.c_str();
    if(!duk_get_prop_string(context, -1, p)) {
        duk_pop(context);
        LogMissingProperty(p);
        return def;
    }

    float var = duk_to_number(context, -1);
    duk_pop(context);

    return var;
}

PLColour ScriptConfig::GetColourProperty(const std::string &property, PLColour def) {
    auto *context = static_cast<duk_context *>(ctx_);

    const char *p = property.c_str();
    if(!duk_get_prop_string(context, -1, p)) {
        duk_pop(context);
        LogMissingProperty(p);
        return def;
    }

    const char *str = duk_safe_to_string(context, -1);
    duk_pop(context);

    PLColour out(255, 255, 255, 255);
    if(std::sscanf(str, "%hhu %hhu %hhu %hhu", &out.r, &out.g, &out.b, &out.a) < 3) {
        // can still ignore alpha channel
        throw std::runtime_error("Failed to parse entirety of colour from JSON property, \"" + property + "\"!\n");
    }

    return out;
}

PLVector3 ScriptConfig::GetVector3Property(const std::string &property, PLVector3 def) {
    auto *context = static_cast<duk_context *>(ctx_);

    const char *p = property.c_str();
    if(!duk_get_prop_string(context, -1, p)) {
        duk_pop(context);
        LogMissingProperty(p);
        return def;
    }

    const char *str = duk_safe_to_string(context, -1);
    duk_pop(context);

    PLVector3 out;
    if(std::sscanf(str, "%f %f %f", &out.x, &out.y, &out.z) < 3) {
        throw std::runtime_error("Failed to parse entirety of vector from JSON property, \"" + property + "\"!\n");
    }

    return out;
}

unsigned int ScriptConfig::GetArrayLength(const std::string &property) {
    auto *context = static_cast<duk_context *>(ctx_);

    const char *p = property.c_str();
    if(!duk_get_prop_string(context, -1, p)) {
        duk_pop(context);
        LogMissingProperty(p);
        return 0;
    }

    if(!duk_is_array(context, -1)) {
        duk_pop(context);
        LogInvalidArray(p);
        return 0;
    }

    duk_size_t len = duk_get_length(context, -1);
    duk_pop(context);

    return static_cast<unsigned int>(len);
}

std::string ScriptConfig::GetArrayStringProperty(const std::string &property, unsigned int index) {
    auto *context = static_cast<duk_context *>(ctx_);

    const char *p = property.c_str();
    if(!duk_get_prop_string(context, -1, p)) {
        duk_pop(context);
        LogMissingProperty(p);
        return "";
    }

    if(!duk_is_array(context, -1)) {
        duk_pop(context);
        LogInvalidArray(p);
        return "";
    }

    duk_size_t length = duk_get_length(context, -1);
    if(index >= length) {
        LogWarn("Invalid index, %d (%d), in array!\n", index, length);
        return "";
    }

    duk_get_prop_string(context, -1, p);
    duk_get_prop_index(context, -1, index);
    const char *str = duk_get_string(context, index);
    duk_pop(context);

    return str;
}


std::vector<std::string> ScriptConfig::GetArrayStrings(const std::string &property) {
    auto *context = static_cast<duk_context *>(ctx_);

    const char *p = property.c_str();
    if(!duk_get_prop_string(context, -1, p)) {
        duk_pop(context);
        LogMissingProperty(p);
        return {};
    }

    if(!duk_is_array(context, -1)) {
        duk_pop(context);
        LogInvalidArray(p);
        return {};
    }

    duk_size_t length = duk_get_length(context, -1);

    std::vector<std::string> strings;
    strings.reserve(length);

    for(unsigned int i = 0; i < length; ++i) {
        duk_get_prop_index(context, -1, i);
        const char *c = duk_get_string(context, -1);
        u_assert(c != nullptr, "Null string passed by duk_get_string!\n");
        strings.emplace_back(c);
        duk_pop(context);
    }

    duk_pop(context);

    return strings;
}

void ScriptConfig::ParseBuffer(const char *buf) {
    if(buf == nullptr) {
        Error("Invalid buffer length!\n");
    }

    auto *context = static_cast<duk_context *>(ctx_);
    duk_push_string(context, buf);
    duk_json_decode(context, -1);
}
