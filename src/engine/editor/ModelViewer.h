// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "BaseWindow.h"

namespace ohw {
	class Camera;
	class ModelViewer : public BaseWindow {
	public:
		explicit ModelViewer();
		~ModelViewer() override;

		void Display() override;

		void DrawViewport();

	protected:
	private:
		void GenerateFrameBuffer( unsigned int width, unsigned int height );

		SharedModelResourcePointer model{ nullptr };
		struct PLGFrameBuffer *drawBuffer{ nullptr };
		struct PLGTexture *textureAttachment{ nullptr };

		Camera *camera{ nullptr };

		PLVector3 modelRotation;

		float oldMousePos[ 2 ]{ 0, 0 };

		bool viewRotate{ true };
		bool viewDebugNormals{ false };
		bool viewSkeleton{ false };
		bool viewGrid{ true };

		static void AppendModelList( const char *path, void *userData );
		static std::list< std::string > modelList;
	};
}
