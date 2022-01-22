// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

namespace ohw {
	class ShaderProgram;
	class Sprite {
	public:
		enum SpriteType {
			TYPE_DEFAULT,    // Depth-tested, scaled manually and oriented
		} type_{ TYPE_DEFAULT };

		Sprite( SpriteType type, const std::string &texturePath, PLColour colour = { 255, 255, 255, 255 }, float scale = 1.0f );
		~Sprite();

		float GetScale() { return scale_; }
		void SetScale( float scale );

		PLVector3 GetPosition() { return position_; }
		void SetPosition( const PLVector3 &position );

		PLVector3 GetAngles() { return angles_; }
		void SetAngles( const PLVector3 &angles );

		PLColour GetColour() { return colour_; }
		void SetColour( const PLColour &colour );

		void SetTexture( const std::string &texturePath );

		//const SpriteAnimation* GetCurrentAnimation() { return current_animation_; }
		//void SetAnimation(SpriteAnimation* anim);

		void Draw();

	protected:
	private:
		PLVector3 position_;
		PLVector3 angles_;
		PLColour colour_{ 255, 255, 255, 255 };
		float scale_{ 1.0f };
		ohw::SharedTextureResourcePointer texture{ nullptr };

		ShaderProgram *defaultProgram;

		unsigned int current_frame_{ 0 };
		double frame_delay_{ 0 };

		PLGMesh *mesh_{ nullptr };
		hei::Matrix4 modelMatrix;
	};
}
