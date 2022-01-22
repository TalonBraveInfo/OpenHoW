// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include "BaseWindow.h"

class ParticleEffect;

namespace ohw {
	class Camera;

	class ParticleEditor : public BaseWindow {
	public:
		explicit ParticleEditor();
		~ParticleEditor() override;

		void Display() override;
		void DrawViewport();

	private:
		void GenerateFrameBuffer( unsigned int width, unsigned int height );

		ParticleEffect *particleEffect{ nullptr };

		struct PLGFrameBuffer *drawBuffer{ nullptr };
		struct PLGTexture *textureAttachment{ nullptr };

		Camera *camera{ nullptr };

		PLVector3 modelRotation{ 0, 0, 0 };

		float oldMousePos[ 2 ]{ 0, 0 };

		bool viewRotate{ true };
		bool viewDebugNormals{ false };
		bool viewGrid{ true };

		static void AppendParticleList( const char *path );
		static std::list< std::string > particleList;
	};
}
