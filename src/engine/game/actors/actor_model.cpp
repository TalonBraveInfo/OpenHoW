/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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

#include "../../engine.h"
#include "../../model.h"
#include "actor_model.h"

using namespace openhow;

AModel::AModel() : SuperClass(),
	INIT_PROPERTY( modelPath, PROP_LOCAL ) {}
AModel::~AModel() = default;

void AModel::Draw() {
	SuperClass::Draw();

	if ( !show_model_ || model_ == nullptr ) {
		return;
	}

	PLVector3 angles(
		plDegreesToRadians( angles_.GetValue().x ),
		plDegreesToRadians( angles_.GetValue().y ),
		plDegreesToRadians( angles_.GetValue().z ) );

	PLMatrix4 mat;
	mat.Identity();
	mat.Rotate( angles.z, { 1, 0, 0 } );
	mat.Rotate( -angles.y, { 0, 1, 0 } );
	mat.Rotate( angles.x, { 0, 0, 1 } );
	mat.Translate( position_ );

	Model_Draw( model_, mat );
}

void AModel::SetModel( const std::string& path ) {
	model_ = Engine::Resource()->LoadModel( "chars/" + path, false );
	u_assert( model_ != nullptr );

	// Keep model path up-to-date
	modelPath = model_->path;
}

void AModel::ShowModel( bool show ) {
	show_model_ = show;
}
