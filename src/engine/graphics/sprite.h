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

class ShaderProgram;

class Sprite {
public:
	enum SpriteType {
		TYPE_DEFAULT,    // Depth-tested, scaled manually and oriented
	} type_{ TYPE_DEFAULT };

	Sprite( SpriteType type, PLTexture* texture, PLColour colour = { 255, 255, 255, 255 }, float scale = 1.0f );
	~Sprite();

	float GetScale() { return scale_; }
	void SetScale( float scale );

	PLVector3 GetPosition() { return position_; }
	void SetPosition( const PLVector3& position );

	PLVector3 GetAngles() { return angles_; }
	void SetAngles( const PLVector3& angles );

	PLColour GetColour() { return colour_; }
	void SetColour( const PLColour& colour );

	void SetTexture( PLTexture* texture );

	//const SpriteAnimation* GetCurrentAnimation() { return current_animation_; }
	//void SetAnimation(SpriteAnimation* anim);

	void Draw();

protected:
private:
	PLVector3 position_;
	PLVector3 angles_;
	PLColour colour_{ 255, 255, 255, 255 };
	float scale_{ 1.0f };

	ShaderProgram* defaultProgram;

	unsigned int current_frame_{ 0 };
	double frame_delay_{ 0 };

	PLMesh* mesh_{ nullptr };
	PLMatrix4 matrix_{};
};
