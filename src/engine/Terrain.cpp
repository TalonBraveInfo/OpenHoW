// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Terrain.h"

#include "graphics/mesh.h"
#include "graphics/ShaderManager.h"
#include "graphics/TextureAtlas.h"
#include "graphics/Camera.h"

//Precalculated vertices for chunk rendering
//TODO: Share one index buffer instance between all chunks
const static unsigned int chunk_indices[96] = {
		0, 2, 1, 1, 2, 3,
		4, 6, 5, 5, 6, 7,
		8, 10, 9, 9, 10, 11,
		12, 14, 13, 13, 14, 15,
		16, 18, 17, 17, 18, 19,
		20, 22, 21, 21, 22, 23,
		24, 26, 25, 25, 26, 27,
		28, 30, 29, 29, 30, 31,
		32, 34, 33, 33, 34, 35,
		36, 38, 37, 37, 38, 39,
		40, 42, 41, 41, 42, 43,
		44, 46, 45, 45, 46, 47,
		48, 50, 49, 49, 50, 51,
		52, 54, 53, 53, 54, 55,
		56, 58, 57, 57, 58, 59,
		60, 62, 61, 61, 62, 63,
};

ohw::Terrain::Terrain( const std::string &tileset ) {
	// attempt to load in the atlas sheet
	// TODO: allow us to change this on the fly
	textureAtlas = new ohw::TextureAtlas( 512, 8 );
	for ( unsigned int i = 0; i < 256; ++i ) {
		if ( !textureAtlas->AddImage( tileset + std::to_string( i ) ) ) {
			break;
		}
	}
	textureAtlas->Finalize();

	chunks_.resize( TERRAIN_CHUNKS );

	Update();
}

ohw::Terrain::~Terrain() {
	delete textureAtlas;

	for ( auto &chunk: chunks_ ) {
		PlgDestroyMesh( chunk.solidMesh );
	}
}

ohw::Terrain::Chunk *ohw::Terrain::GetChunk( const PLVector2 &pos ) {
	if ( pos.x < 0 || std::floor( pos.x ) >= TERRAIN_PIXEL_WIDTH || pos.y < 0 || std::floor( pos.y ) >= TERRAIN_PIXEL_WIDTH ) {
		return nullptr;
	}

	unsigned int idx =
			( ( unsigned int ) ( pos.x ) / TERRAIN_CHUNK_PIXEL_WIDTH ) +
			( ( ( unsigned int ) ( pos.y ) / TERRAIN_CHUNK_PIXEL_WIDTH ) * TERRAIN_CHUNK_ROW );
	if ( idx >= chunks_.size() ) {
		Warning( "Attempted to get an out of bounds chunk index (%d)!\n", idx );
		return nullptr;
	}

	return &chunks_[ idx ];
}

ohw::Terrain::Tile *ohw::Terrain::GetTile( float x, float y ) {
	Chunk *chunk = GetChunk( { x, y } );
	if ( chunk == nullptr ) {
		return nullptr;
	}

	unsigned int idx =
			( ( ( unsigned int ) ( x ) / TERRAIN_TILE_PIXEL_WIDTH ) % TERRAIN_CHUNK_ROW_TILES ) +
			( ( ( ( unsigned int ) ( y ) / TERRAIN_TILE_PIXEL_WIDTH ) % TERRAIN_CHUNK_ROW_TILES ) * TERRAIN_CHUNK_ROW_TILES );
	if ( idx >= TERRAIN_CHUNK_TILES ) {
		Warning( "Attempted to get an out of bounds tile index!\n" );
		return nullptr;
	}

	return &chunk->tiles[ idx ];
}

/**
 * Return the height at the given point. If we fail to find a tile there, we just return 0.
 */
float ohw::Terrain::GetHeight( float x, float y ) {
	const Tile *tile = GetTile( x, y );
	if ( tile == nullptr ) {
		return 0;
	}

	float tile_x = x - std::floor( x );
	float tile_y = y - std::floor( y );

	float nx = tile->height[ 0 ] + ( ( tile->height[ 1 ] - tile->height[ 0 ] ) * tile_x );
	float ny = tile->height[ 2 ] + ( ( tile->height[ 3 ] - tile->height[ 2 ] ) * tile_x );
	float nz = nx + ( ( ny - nx ) * tile_y );

	return nz;
}

void ohw::Terrain::GenerateChunkMesh( Chunk *chunk, const PLVector2 &offset ) {
	// We will need to generate the mesh for the cunk again if the terrain is modified
	if ( chunk->solidMesh != nullptr ) {
		PlgDestroyMesh( chunk->solidMesh );
	}

	chunk->solidMesh = PlgCreateMeshInit( PLG_MESH_TRIANGLES, PLG_DRAW_DYNAMIC, 32, 64, chunk_indices, nullptr );
	if ( chunk->solidMesh == nullptr ) {
		Error( "Unable to create map chunk mesh, aborting!\nPL: %s\n", PlGetError() );
	}

	if ( chunk->waterMesh != nullptr ) {
		PlgDestroyMesh( chunk->waterMesh );
	}

	chunk->waterMesh = PlgCreateMeshInit( PLG_MESH_TRIANGLES, PLG_DRAW_DYNAMIC, 32, 64, chunk_indices, nullptr );
	if ( chunk->waterMesh == nullptr ) {
		Error( "Unable to create water chunk mesh, aborting!\nPL: %s\n", PlGetError() );
	}

	int cm_idx = 0;
	unsigned int numWaterTiles = 0;
	for ( unsigned int tile_y = 0; tile_y < TERRAIN_CHUNK_ROW_TILES; ++tile_y ) {
		for ( unsigned int tile_x = 0; tile_x < TERRAIN_CHUNK_ROW_TILES; ++tile_x ) {
			const Tile *current_tile = &chunk->tiles[ tile_x + tile_y * TERRAIN_CHUNK_ROW_TILES ];

			float tx_x, tx_y, tx_w, tx_h;
			textureAtlas->GetTextureCoords( std::to_string( current_tile->texture ), &tx_x, &tx_y, &tx_w, &tx_h );

			// TERRAIN_FLIP_FLAG_X flips around texture sheet coords, not TERRAIN coords.
			if ( current_tile->rotation & Tile::ROTATION_FLAG_X ) {
				tx_x = tx_x + tx_w;
				tx_w = -tx_w;
			}

			// ST coords for each corner of the tile.
			float tx_Ax[] = { tx_x, tx_x + tx_w, tx_x, tx_x + tx_w };
			float tx_Ay[] = { tx_y, tx_y, tx_y + tx_h, tx_y + tx_h };

			// Rotate a quad of ST coords 90 degrees clockwise.
			auto rot90 = []( float *x ) {
				float c = x[ 0 ];
				x[ 0 ] = x[ 2 ];
				x[ 2 ] = x[ 3 ];
				x[ 3 ] = x[ 1 ];
				x[ 1 ] = c;
			};

			if ( current_tile->rotation & Tile::ROTATION_FLAG_ROTATE_90 ) {
				rot90( tx_Ax );
				rot90( tx_Ay );
			}

			if ( current_tile->rotation & Tile::ROTATION_FLAG_ROTATE_180 ) {
				rot90( tx_Ax );
				rot90( tx_Ay );
				rot90( tx_Ax );
				rot90( tx_Ay );
			}

			// MAP_FLIP_FLAG_ROTATE_270 is implemented by ORing 90 and 180 together.

			for ( int i = 0; i < 4; ++i, ++cm_idx ) {
				float x = ( offset.x * TERRAIN_CHUNK_PIXEL_WIDTH ) + ( tile_x + ( i % 2 ) ) * TERRAIN_TILE_PIXEL_WIDTH;
				float z = ( offset.y * TERRAIN_CHUNK_PIXEL_WIDTH ) + ( tile_y + ( i / 2 ) ) * TERRAIN_TILE_PIXEL_WIDTH;

				PLVector3 position = hei::Vector3( x, current_tile->height[ i ], z );
				PLColour shadedColour = hei::Colour( current_tile->shading[ i ], current_tile->shading[ i ], current_tile->shading[ i ] );

				PlgSetMeshVertexPosition( chunk->solidMesh, cm_idx, position );
				PlgSetMeshVertexColour( chunk->solidMesh, cm_idx, shadedColour );
				PlgSetMeshVertexST( chunk->solidMesh, cm_idx, tx_Ax[ i ], tx_Ay[ i ] );

				if ( current_tile->behaviour == Tile::BEHAVIOUR_WATERY ) {
					shadedColour.a = 145;
					PlgSetMeshVertexPosition( chunk->waterMesh, cm_idx, position );
					PlgSetMeshVertexColour( chunk->waterMesh, cm_idx, shadedColour );
					PlgSetMeshVertexST( chunk->waterMesh, cm_idx, tx_Ax[ i ], tx_Ay[ i ] );
					numWaterTiles++;
				}
			}
		}
	}

	if ( numWaterTiles == 0 ) {
		PlgDestroyMesh( chunk->waterMesh );
		chunk->waterMesh = nullptr;
	}
}

void ohw::Terrain::GenerateOverview() {
	static const PLColour colours[] = {
			{ 60,  50,  40 },     // Mud
			{ 40,  70,  40 },     // Grass
			{ 128, 128, 128 },  // Metal
			{ 153, 94,  34 },    // Wood
			{ 90,  90,  150 },    // Water
			{ 50,  50,  50 },     // Stone
			{ 50,  50,  50 },     // Rock
			{ 100, 80,  30 },    // Sand
			{ 180, 240, 240 },  // Ice
			{ 100, 100, 100 },  // Snow
			{ 60,  50,  40 },     // Quagmire
			{ 100, 240, 53 }    // Lava/Poison
	};

	// Create our storage
	PLImage *image = PlCreateImage( nullptr, 64, 64, PL_COLOURFORMAT_RGB, PL_IMAGEFORMAT_RGB8 );

	// Now write into the image buffer
	uint8_t *buf = image->data[ 0 ];
	for ( uint8_t y = 0; y < 64; ++y ) {
		for ( uint8_t x = 0; x < 64; ++x ) {
			hei::Vector2 position( x * ( TERRAIN_PIXEL_WIDTH / 64 ), y * ( TERRAIN_PIXEL_WIDTH / 64 ) );
			const Tile *tile = GetTile( position.x, position.y );
			u_assert( tile != nullptr, "Hit an invalid tile during overview generation!\n" );
			if ( tile == nullptr ) {
				continue;
			}

			auto mod = static_cast<int>(( GetHeight( position.x, position.y ) + ( ( GetMaxHeight() + GetMinHeight() ) / 2 ) ) / 255);
			PLColour rgb = hei::Colour(
					std::min( ( colours[ tile->surface ].r / 9 ) * mod, 255 ),
					std::min( ( colours[ tile->surface ].g / 9 ) * mod, 255 ),
					std::min( ( colours[ tile->surface ].b / 9 ) * mod, 255 )
			);
			if ( tile->behaviour & Tile::BEHAVIOUR_MINE ) {
				rgb = { 255, 0, 0 };
			}

			*( buf++ ) = rgb.r;
			*( buf++ ) = rgb.g;
			*( buf++ ) = rgb.b;
		}
	}

#if !defined( NDEBUG )
	if ( PlCreatePath( "./debug/generated/" ) ) {
		char path[PL_SYSTEM_MAX_PATH];
		static unsigned int id = 0;
		snprintf( path, sizeof( path ) - 1, "./debug/generated/%dx%d_%d.png", image->width, image->height, id );
		PlWriteImage( image, path );
	}
#endif

	// Allow rebuilding overview texture
	PlgDestroyTexture( overview_ );

	if ( ( overview_ = PlgCreateTexture() ) == nullptr ) {
		Error( "Failed to generate overview texture slot!\n%s\n", PlGetError() );
	}

	PlgUploadTextureImage( overview_, image );
	PlDestroyImage( image );
}

void ohw::Terrain::Update() {
	GenerateOverview();

	for ( unsigned int chunk_y = 0; chunk_y < TERRAIN_CHUNK_ROW; ++chunk_y ) {
		for ( unsigned int chunk_x = 0; chunk_x < TERRAIN_CHUNK_ROW; ++chunk_x ) {
			GenerateChunkMesh( &chunks_[ chunk_x + chunk_y * TERRAIN_CHUNK_ROW ],
			                   { static_cast<float>(chunk_x), static_cast<float>(chunk_y) } );
		}
	}

	std::list< PLGMesh * > meshes;
	for ( auto &chunk: chunks_ ) {
		if ( chunk.solidMesh != nullptr ) {
			meshes.push_back( chunk.solidMesh );
		}
		if ( chunk.waterMesh != nullptr ) {
			meshes.push_back( chunk.waterMesh );
		}
	}

	if ( !meshes.empty() ) {
		Mesh_GenerateFragmentedMeshNormals( meshes );
	}
}

void ohw::Terrain::Draw() {
	ohw::Camera *cameraPtr = GetApp()->gameManager->GetActiveCamera();
	if ( cameraPtr == nullptr ) {
		return;
	}

	if ( !cv_graphics_debug_normals->b_value ) {
		PlgSetTexture( textureAtlas->GetTexture(), 0 );
	}

	PlMatrixMode( PL_MODELVIEW_MATRIX );
	PlPushMatrix();

	PlLoadIdentityMatrix();

	PLGShaderProgram *program;

	// Solid
	program = Shaders_GetProgram( cv_graphics_debug_normals->b_value ? "debug_normals" : "generic_textured_lit" )->GetInternalProgram();
	if ( program != nullptr ) {
		PlgSetShaderProgram( program );
		PlgSetShaderUniformValue( program, "pl_model", PlGetMatrix( PL_MODELVIEW_MATRIX ), true );

		PlgSetBlendMode( PLG_BLEND_DISABLE );

		for ( const auto &chunk: chunks_ ) {
			if ( ( cv_graphics_cull->b_value && !cameraPtr->IsBoxVisible( &chunk.bounds ) ) || chunk.solidMesh == nullptr ) {
				continue;
			}

			PlgUploadMesh( chunk.solidMesh );
			PlgDrawMesh( chunk.solidMesh );
		}
	}

	if ( !cv_graphics_debug_normals->b_value ) {
		// Water
		program = Shaders_GetProgram( "water" )->GetInternalProgram();
		if ( program != nullptr ) {
			PlgSetShaderProgram( program );
			PlgSetShaderUniformValue( program, "pl_model", PlGetMatrix( PL_MODELVIEW_MATRIX ), true );

			PlgSetBlendMode( PLG_BLEND_DEFAULT );

			for ( const auto &chunk: chunks_ ) {
				if ( ( cv_graphics_cull->b_value && !cameraPtr->IsBoxVisible( &chunk.bounds ) ) || chunk.waterMesh == nullptr ) {
					continue;
				}

				PlgUploadMesh( chunk.waterMesh );
				PlgDrawMesh( chunk.waterMesh );
			}
		}

		if ( cv_debug_bounds->b_value ) {
			Shaders_SetProgramByName( "generic_untextured" );

			for ( const auto &chunk: chunks_ ) {
				if ( ( cv_graphics_cull->b_value && !cameraPtr->IsBoxVisible( &chunk.bounds ) ) ) {
					continue;
				}

				PlgDrawBoundingVolume( &chunk.bounds, PL_COLOUR_ORANGE );
			}
		}
	}

	if ( !cv_graphics_debug_normals->b_value ) {
		PlgSetTexture( nullptr, 0 );
	}

	PlPopMatrix();
}

void ohw::Terrain::LoadPmg( const std::string &path ) {
	PLFile *fh = PlOpenFile( path.c_str(), false );
	if ( fh == nullptr ) {
		Warning( "Failed to open tile data, \"%s\", aborting\n", path.c_str() );
		return;
	}

	for ( unsigned int chunk_y = 0; chunk_y < TERRAIN_CHUNK_ROW; ++chunk_y ) {
		for ( unsigned int chunk_x = 0; chunk_x < TERRAIN_CHUNK_ROW; ++chunk_x ) {
			Chunk &chunk = chunks_[ chunk_x + chunk_y * TERRAIN_CHUNK_ROW ];

			bool status;
			chunk.x = PlReadInt16( fh, false, &status );
			chunk.y = PlReadInt16( fh, false, &status );
			chunk.z = PlReadInt16( fh, false, &status );
			/* int16_t unknown0 = */
			PlReadInt16( fh, false, &status );

			if ( !status ) {
				Error( "Failed to read in chunk descriptor in \"%s\"!\n", PlGetFilePath( fh ) );
			}

			chunk.bounds.origin = hei::Vector3( chunk_x * TERRAIN_CHUNK_PIXEL_WIDTH, 0.0f, chunk_y * TERRAIN_CHUNK_PIXEL_WIDTH );
			chunk.bounds.maxs.z = chunk.bounds.maxs.x = TERRAIN_CHUNK_PIXEL_WIDTH;
			chunk.bounds.mins.z = chunk.bounds.mins.x = -TERRAIN_CHUNK_PIXEL_WIDTH;

			struct {
				int16_t height{ 0 };
				uint16_t lighting{ 0 };
			} vertices[25];

			// Find the maximum and minimum points
			chunk.bounds.maxs.y = INT16_MIN;
			chunk.bounds.mins.y = INT16_MAX;
			for ( auto &vertex: vertices ) {
				vertex.height = PlReadInt16( fh, false, &status );
				vertex.lighting = PlReadInt16( fh, false, &status );

				if ( !status ) {
					Error( "Failed to read in vertex descriptor in \"%s\"!\n", PlGetFilePath( fh ) );
				}

				// Determine the maximum height and minimum height for this chunk
				if ( vertex.height > chunk.bounds.maxs.y ) {
					chunk.bounds.maxs.y = vertex.height;
				}
				if ( vertex.height < chunk.bounds.mins.y ) {
					chunk.bounds.mins.y = vertex.height;
				}
				// And now for the entire terrain
				if ( static_cast<float>(vertex.height) > max_height_ ) {
					max_height_ = vertex.height;
				}
				if ( static_cast<float>(vertex.height) < min_height_ ) {
					min_height_ = vertex.height;
				}
			}

			PlFileSeek( fh, 4, PL_SEEK_CUR );

			for ( unsigned int tile_y = 0; tile_y < TERRAIN_CHUNK_ROW_TILES; ++tile_y ) {
				for ( unsigned int tile_x = 0; tile_x < TERRAIN_CHUNK_ROW_TILES; ++tile_x ) {
					struct {
						int8_t unused0[6]{ 0, 0, 0, 0, 0, 0 };
						uint8_t type{ 0 };
						uint8_t slip{ 0 };
						int16_t unused1{ 0 };
						uint8_t rotation{ 0 };
						uint32_t texture{ 0 };
						uint8_t unused2{ 0 };
					} tile;

					// Skip unused data
					if ( PlReadFile( fh, tile.unused0, 6, 1 ) != 1 ) {
						Error( "Failed to skip unused bytes in \"%s\"!\n", PlGetFilePath( fh ) );
					}

					tile.type = PlReadInt8( fh, &status );
					tile.slip = PlReadInt8( fh, &status );
					tile.unused1 = PlReadInt16( fh, false, &status );
					tile.rotation = PlReadInt8( fh, &status );
					tile.texture = PlReadInt32( fh, false, &status );
					tile.unused2 = PlReadInt8( fh, &status );

					if ( !status ) {
						Error( "Failed to read in tile descriptor in \"%s\"!\n", PlGetFilePath( fh ) );
					}

					Tile *current_tile = &chunk.tiles[ tile_x + tile_y * TERRAIN_CHUNK_ROW_TILES ];
					current_tile->surface = static_cast<Tile::Surface>(tile.type & 31U);
					current_tile->behaviour = static_cast<Tile::Behaviour>(tile.type & ~31U);
					current_tile->rotation = static_cast<Tile::Rotation>(tile.rotation);
					current_tile->slip = 0;
					current_tile->texture = tile.texture;

					current_tile->height[ 0 ] = vertices[ ( tile_y * 5 ) + tile_x ].height;
					current_tile->height[ 1 ] = vertices[ ( tile_y * 5 ) + tile_x + 1 ].height;
					current_tile->height[ 2 ] = vertices[ ( ( tile_y + 1 ) * 5 ) + tile_x ].height;
					current_tile->height[ 3 ] = vertices[ ( ( tile_y + 1 ) * 5 ) + tile_x + 1 ].height;

					current_tile->shading[ 0 ] = vertices[ ( tile_y * 5 ) + tile_x ].lighting;
					current_tile->shading[ 1 ] = vertices[ ( tile_y * 5 ) + tile_x + 1 ].lighting;
					current_tile->shading[ 2 ] = vertices[ ( ( tile_y + 1 ) * 5 ) + tile_x ].lighting;
					current_tile->shading[ 3 ] = vertices[ ( ( tile_y + 1 ) * 5 ) + tile_x + 1 ].lighting;
				}
			}
		}
	}

	PlCloseFile( fh );

	Update();
}

void ohw::Terrain::LoadHeightmap( const std::string &path, int multiplier ) {
	PLImage *image = PlLoadImage( path.c_str() );
	if ( image == nullptr ) {
		Warning( "Failed to load the specified heightmap, \"%s\" (%s)!\n", path.c_str(), PlGetError() );
		return;
	}

	if ( image->width < 65 || image->height < 65 ) {
		PlDestroyImage( image );
		Warning( "Invalid image size for heightmap, %dx%d vs 65x65!\n", image->width, image->height );
		return;
	}

	// Each channel is encoded with specific data
	// red = height
	// green = texture

	unsigned int chan_length = image->width * image->height;

	auto *rchan = static_cast<float *>(u_alloc( chan_length, sizeof( float ), true ));
	uint8_t *pixel = image->data[ 0 ];
	for ( unsigned int i = 0; i < chan_length; ++i ) {
		rchan[ i ] = static_cast<int>(*pixel) * multiplier; //(static_cast<int>(*pixel) - 127) * 256;
		pixel += 4;
	}

	auto *gchan = static_cast<uint8_t *>(u_alloc( chan_length, sizeof( uint8_t ), true ));
	pixel = image->data[ 0 ] + 1;
	for ( unsigned int i = 0; i < chan_length; ++i ) {
		gchan[ i ] = *pixel;
		pixel += 4;
	}

	PlDestroyImage( image );

	for ( unsigned int chunk_y = 0; chunk_y < TERRAIN_CHUNK_ROW; ++chunk_y ) {
		for ( unsigned int chunk_x = 0; chunk_x < TERRAIN_CHUNK_ROW; ++chunk_x ) {
			Chunk &current_chunk = chunks_[ chunk_x + chunk_y * TERRAIN_CHUNK_ROW ];
			for ( unsigned int tile_y = 0; tile_y < TERRAIN_CHUNK_ROW_TILES; ++tile_y ) {
				for ( unsigned int tile_x = 0; tile_x < TERRAIN_CHUNK_ROW_TILES; ++tile_x ) {
					Tile *current_tile = &current_chunk.tiles[ tile_x + tile_y * TERRAIN_CHUNK_ROW_TILES ];
					unsigned int aaa = ( chunk_y * 4 * 65 ) + ( chunk_x * 4 );
					current_tile->height[ 0 ] = rchan[ aaa + ( tile_y * 65 ) + tile_x ];
					current_tile->height[ 1 ] = rchan[ aaa + ( tile_y * 65 ) + tile_x + 1 ];
					current_tile->height[ 2 ] = rchan[ aaa + ( ( tile_y + 1 ) * 65 ) + tile_x ];
					current_tile->height[ 3 ] = rchan[ aaa + ( ( tile_y + 1 ) * 65 ) + tile_x + 1 ];

					current_tile->texture = gchan[ aaa + ( tile_y * 65 ) + tile_x ];

					// hrm...
					current_tile->shading[ 0 ] =
					current_tile->shading[ 1 ] =
					current_tile->shading[ 2 ] =
					current_tile->shading[ 3 ] = 255;
				}
			}

			max_height_ = min_height_ = current_chunk.tiles[ 0 ].height[ 0 ];

			// Find the maximum and minimum points
			for ( auto &tile: current_chunk.tiles ) {
				for ( float i: tile.height ) {
					if ( i > max_height_ ) {
						max_height_ = i;
					}
					if ( i < min_height_ ) {
						min_height_ = i;
					}
				}
			}
		}
	}

	u_free( rchan );
	u_free( gchan );

	Update();
}
