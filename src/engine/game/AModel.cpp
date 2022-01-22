// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
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

	hei::Vector3 angles(
			PlDegreesToRadians( myAngles.GetValue().x ),
			PlDegreesToRadians( myAngles.GetValue().y ),
			PlDegreesToRadians( myAngles.GetValue().z ) );

	hei::Matrix4 mat;
	mat.Identity();
	mat.Rotate( angles.z, { 1, 0, 0 } );
	mat.Rotate( -angles.y, { 0, 1, 0 } );
	mat.Rotate( angles.x, { 0, 0, 1 } );
	mat.Translate( position_ );

	model->modelMatrix = mat;
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
