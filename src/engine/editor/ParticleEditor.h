/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

		struct PLFrameBuffer *drawBuffer{ nullptr };
		struct PLTexture *textureAttachment{ nullptr };

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
