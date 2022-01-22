// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include <plgraphics/plg.h>

namespace ohw {
	class ShaderProgram {
	public:
		ShaderProgram( const std::string &vertPath, const std::string &fragPath );
		~ShaderProgram();

		void Rebuild();

		void Enable();
		void Disable();

		// This is awful, but we need it for passing shader programs into
		// the platform library. Urgh...
		PLGShaderProgram *GetInternalProgram() const { return shaderProgram; }

	private:
		void RegisterShaderStage( const char *path, PLGShaderStageType type );

		std::string vertPath;
		std::string fragPath;

		PLGShaderProgram *shaderProgram{ nullptr };
	};
}

ohw::ShaderProgram* Shaders_GetProgram( const std::string& name );

void Shaders_SetProgramByName( const std::string& name );

void Shaders_Initialize();
void Shaders_Shutdown();
