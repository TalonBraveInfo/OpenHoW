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
#include "Map.h"

#include "graphics/ShaderManager.h"
#include "graphics/TextureAtlas.h"

ohw::Map::Map( MapManifest *manifest ) : manifest_( manifest ) {
	std::string base_path = "maps/" + manifest_->filename + "/";

	std::string tilePath = manifest_->tile_directory;
	if ( tilePath.empty() ) {
		// Fallback to maps/<name>/tiles/ if not specified
		tilePath = "maps/" + manifest_->filename + "/tiles/";
	}

	// create the terrain
	terrain_ = new Terrain( tilePath );
	// then load the Pmg if it exists otherwise
	// we'll just assume it's a new map (heightmap data can be imported after)
	std::string pmgPath = "maps/" + manifest_->filename + "/" + manifest_->filename + ".pmg";
	terrain_->LoadPmg( pmgPath );

	std::string pogPath = "maps/" + manifest_->filename + "/" + manifest_->filename + ".pog";
	LoadSpawns( pogPath );

	// Load both the bottom and top parts of the sky dome
	if ( skyModelTop == nullptr ) {
		skyModelTop = LoadSkyModel( "skys/skydome.vtx" );
	}
	if ( skyModelBottom == nullptr ) {
		skyModelBottom = LoadSkyModel( "skys/skydomeu.vtx" );
	}

	UpdateSky();
}

ohw::Map::~Map() {
	delete terrain_;
}

ohw::SharedModelResourcePointer ohw::Map::LoadSkyModel( const std::string &path ) {
	SharedModelResourcePointer model = GetApp()->resourceManager->LoadModel( path, true, true );

	// Default skydome is smaller than the map, so we'll scale it
	model->modelMatrix.Identity();
	model->modelMatrix.Translate( PLVector3( TERRAIN_PIXEL_WIDTH / 2.0f, 0, TERRAIN_PIXEL_WIDTH / 2.0f ) );
	model->modelMatrix *= PLVector3( 5.0f, 5.0f, 5.0f );

	PLMesh *mesh = model->GetInternalMesh( 0 );

	// This is a really crap hardcoded limit, just to ensure it's what we're expecting
	if ( mesh->num_verts != 257 ) {
		Error( "Unexpected number of vertices for sky mesh! (%d vs 257)\n", mesh->num_verts );
	}

	return model;
}

void ohw::Map::UpdateSky() {
	UpdateSkyModel( skyModelTop );
	UpdateSkyModel( skyModelBottom );
}

void ohw::Map::UpdateSkyModel( SharedModelResourcePointer model ) {
	PLMesh *mesh = model->GetInternalMesh( 0 );

	// Below is a PSX-style gradient sky implementation
	const unsigned int solid_steps = 3;
	const unsigned int grad_steps = 6;
	PLColour top = manifest_->sky_colour_top;
	PLColour bottom = manifest_->sky_colour_bottom;
	int stepr = ( ( int ) ( bottom.r ) - ( int ) ( top.r ) ) / ( int ) ( grad_steps );
	int stepg = ( ( int ) ( bottom.g ) - ( int ) ( top.g ) ) / ( int ) ( grad_steps );
	int stepb = ( ( int ) ( bottom.b ) - ( int ) ( top.b ) ) / ( int ) ( grad_steps );

	if ( stepr < 0 ) { stepr += 255; }
	if ( stepg < 0 ) { stepg += 255; }
	if ( stepb < 0 ) { stepb += 255; }

	PLColour colour = top;
	for ( unsigned int i = 0, j = 31, s = 0; i < mesh->num_verts; ++i, ++j ) {
		if ( j == 32 ) {
			if ( ++s >= solid_steps ) {
				colour.r += stepr;
				colour.g += stepg;
				colour.b += stepb;
			}
			j = 0;
		}

		mesh->vertices[ i ].colour = colour;
	}

	plUploadMesh( mesh );
}

/**
 * Fetches a random spot in the playable area of the map.
 */
PLVector2 ohw::Map::GetRandomPointInPlayArea() const {
	return PLVector2(
			TERRAIN_PLAYABLE_BORDER + plGenerateRandomf( TERRAIN_PLAYABLE_AREA ),
			TERRAIN_PLAYABLE_BORDER + plGenerateRandomf( TERRAIN_PLAYABLE_AREA ) );
}

/**
 * Fetch the bounds of the playable area.
 */
void ohw::Map::GetPlayArea( PLVector2 *min, PLVector2 *max ) const {
	min->x = TERRAIN_PLAYABLE_BORDER;
	max->x = TERRAIN_PLAYABLE_AREA;
	min->y = TERRAIN_PLAYABLE_BORDER;
	max->y = TERRAIN_PLAYABLE_AREA;
}

void ohw::Map::LoadSpawns( const std::string &path ) {
	struct PogIndex {
		char name[16];               // class name
		char unused0[16];
		int16_t position[3];            // position in the world
		uint16_t index;                  // todo
		int16_t angles[3];              // angles in the world
		uint16_t type;                   // todo
		int16_t bounds[3];              // collision bounds
		uint16_t bounds_type;            // box, prism, sphere and none
		int16_t energy;
		uint8_t appearance;
		uint8_t team;                   // uk, usa, german, french, japanese, soviet
		uint16_t objective;
		uint8_t objective_actor_id;
		uint8_t objective_extra[2];
		uint8_t unused1;
		uint16_t unused2[8];
		int16_t fallback_position[3];
		int16_t extra;
		int16_t attached_actor_num;
		int16_t unused3;
	};
	static_assert( sizeof( PogIndex ) == 94, "Invalid size for PogIndex, should be 94 bytes!" );

	const char *cPath = path.c_str();
	PLFile *fp = plOpenFile( cPath, false );
	if ( fp == NULL ) {
		Warning( "Failed to open actor data, \"%s\" (%s)!\n", cPath, plGetError() );
		return;
	}

	bool status = false;
	uint16_t num_indices = plReadInt16( fp, false, &status );
	if ( !status ) {
		Error( "Failed to read Pog indices count in \"%s\" (%s)!\n", cPath, plGetError() );
	}

	std::vector< PogIndex > spawns( num_indices );
	if ( plReadFile( fp, spawns.data(), sizeof( PogIndex ), num_indices ) != num_indices ) {
		Error( "Failed to read Pog spawns in \"%s\" (%s)!\n", cPath, plGetError() );
	}

	plCloseFile( fp );

	actorSpawns.resize( num_indices );

	for ( unsigned int i = 0; i < num_indices; ++i ) {
		actorSpawns[ i ].position.x = static_cast<float>(spawns[ i ].position[ 0 ] + ( TERRAIN_PIXEL_WIDTH / 2 ));
		actorSpawns[ i ].position.y = static_cast<float>(spawns[ i ].position[ 1 ]);
		actorSpawns[ i ].position.z = static_cast<float>(( spawns[ i ].position[ 2 ] * -1 ) + ( TERRAIN_PIXEL_WIDTH / 2 ));

		actorSpawns[ i ].fallbackPosition.x = ( spawns[ i ].fallback_position[ 0 ] += ( TERRAIN_PIXEL_WIDTH / 2 ) );
		actorSpawns[ i ].fallbackPosition.y = ( spawns[ i ].fallback_position[ 1 ] );
		actorSpawns[ i ].fallbackPosition.z = ( spawns[ i ].fallback_position[ 2 ] += ( TERRAIN_PIXEL_WIDTH / 2 ) * -1 );

		actorSpawns[ i ].angles.x = ( float ) ( spawns[ i ].angles[ 0 ] ) * ( 360.f / 4096.f );
		actorSpawns[ i ].angles.y = ( float ) ( spawns[ i ].angles[ 1 ] ) * ( 360.f / 4096.f ) - 90.0f;
		actorSpawns[ i ].angles.z = ( float ) ( spawns[ i ].angles[ 2 ] ) * ( 360.f / 4096.f ) + 180.0f;

		actorSpawns[ i ].className = u_stringtolower( spawns[ i ].name );
		actorSpawns[ i ].appearance = spawns[ i ].appearance;

		try {
			actorSpawns[ i ].attachment = &actorSpawns.at( spawns[ i ].attached_actor_num );
		} catch ( const std::out_of_range &e ) {
			Warning( "Failed to get valid attachment for spawn (%s, %s)!\n", actorSpawns[ i ].className.c_str(),
			         plPrintVector3( &actorSpawns[ i ].position, pl_int_var ) );
		}

		actorSpawns[ i ].energy = spawns[ i ].energy;
		actorSpawns[ i ].index = spawns[ i ].index;

		// todo: retain or set on per-actor basis?
		for ( unsigned int j = 0; j < 3; ++j ) {
			actorSpawns[ i ].bounds[ j ] = spawns[ i ].bounds[ j ];
		}
		actorSpawns[ i ].bounds_type = spawns[ i ].bounds_type;
	}
}

void ohw::Map::Draw() {
	Shaders_SetProgramByName( "generic_untextured" );

	skyModelTop->Draw( false );
	skyModelBottom->Draw( false );

#if 1
	PLCollisionAABB bounds;

	bounds.origin.x = TERRAIN_PLAYABLE_BORDER;
	bounds.origin.y = 512.0f;
	bounds.origin.z = TERRAIN_PLAYABLE_BORDER;

	bounds.maxs.x = TERRAIN_PLAYABLE_AREA;
	bounds.maxs.y = terrain_->GetMaxHeight();
	bounds.maxs.z = TERRAIN_PLAYABLE_AREA;

	bounds.mins.x = 0;
	bounds.mins.y = terrain_->GetMinHeight();
	bounds.mins.z = 0;

	plDrawBoundingVolume( &bounds, PL_COLOUR_RED );
#endif

	if ( !cv_GraphicsDrawTerrain->b_value ) {
		return;
	}

	// TODO: move this somewhere else???
	PLShaderProgram *program = Shaders_GetProgram( "generic_textured_lit" )->GetInternalProgram();
	if ( program == nullptr ) {
		return;
	}

	PLVector4 vColour = manifest_->fog_colour.ToVec4();
	plSetShaderUniformValue( program, "fog_colour", &vColour, false );
	plSetShaderUniformValue( program, "fog_near", &manifest_->fog_intensity, false );
	plSetShaderUniformValue( program, "fog_far", &manifest_->fog_distance, false );

	PLVector3 sunPosition( 1.0f, -manifest_->sun_pitch, 0 );
	PLMatrix4 sunMatrix;
	sunMatrix.Identity();
	sunMatrix.Translate( sunPosition );
	sunMatrix.Rotate( manifest_->sun_yaw, PLVector3( 0.0f, 1.0f, 0.0f ) );
	sunPosition.x = sunMatrix.m[ 0 ];
	sunPosition.z = sunMatrix.m[ 8 ];

	plSetShaderUniformValue( program, "sun_position", &sunPosition, false );
	vColour = manifest_->sun_colour.ToVec4();
	plSetShaderUniformValue( program, "sun_colour", &vColour, false );
	vColour = manifest_->ambient_colour.ToVec4();
	plSetShaderUniformValue( program, "ambient_colour", &vColour, false );

	terrain_->Draw();
}
