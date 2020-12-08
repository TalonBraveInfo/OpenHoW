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

#include "App.h"
#include "model.h"
#include "AModel.h"

using namespace ohw;

AModel::AModel() : SuperClass(),
                   INIT_PROPERTY( modelPath, PROP_LOCAL ) {}
AModel::~AModel() = default;

void AModel::Draw() {
	SuperClass::Draw();

	if ( !show_model_ || model == nullptr ) {
		return;
	}

	PLVector3 angles(
			plDegreesToRadians( myAngles.GetValue().x ),
			plDegreesToRadians( myAngles.GetValue().y ),
			plDegreesToRadians( myAngles.GetValue().z ) );

	model->modelMatrix.Identity();
	model->modelMatrix.Rotate( angles.z, { 1, 0, 0 } );
	model->modelMatrix.Rotate( -angles.y, { 0, 1, 0 } );
	model->modelMatrix.Rotate( angles.x, { 0, 0, 1 } );
	model->modelMatrix.Translate( position_ );

	model->Draw();
}

void AModel::SetModel( const std::string &path ) {
	model = GetApp()->resourceManager->LoadModel( "chars/" + path, false );
	u_assert( model != nullptr );

	// Keep model path up-to-date
	modelPath = model->GetPath();
}

void AModel::ShowModel( bool show ) {
	show_model_ = show;
}
