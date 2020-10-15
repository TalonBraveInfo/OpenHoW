/* OpenHoW
 * Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
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

#include <PL/platform_filesystem.h>
#include <PL/platform_image.h>
#include <PL/platform_console.h>

/**
 * Img2Pmg
 * This tool will convert any four channel image into a PMG file
 * for use with Hogs of War.
 *
 * R = Height
 * G = Texture
 * B = Surface
 * A = Shading
 */

#define Print(...)  plLogMessage(0, __VA_ARGS__)
#define Warning(...)  plLogMessage(1, __VA_ARGS__)
#define Error(...)    plLogMessage(2, __VA_ARGS__); exit(EXIT_FAILURE)

/* ripped from terrain.h */
#define TERRAIN_CHUNK_ROW           16
#define TERRAIN_CHUNKS              (TERRAIN_CHUNK_ROW * TERRAIN_CHUNK_ROW)
#define TERRAIN_CHUNK_ROW_TILES     4
#define TERRAIN_CHUNK_TILES         (TERRAIN_CHUNK_ROW_TILES * TERRAIN_CHUNK_ROW_TILES)
#define TERRAIN_TILE_PIXEL_WIDTH    512
#define TERRAIN_CHUNK_PIXEL_WIDTH   2048
#define TERRAIN_ROW_TILES           (TERRAIN_CHUNK_ROW * TERRAIN_CHUNK_ROW_TILES)
#define TERRAIN_PIXEL_WIDTH         (TERRAIN_TILE_PIXEL_WIDTH * TERRAIN_ROW_TILES)

typedef struct PmgTile {
  unsigned int surface;
  unsigned int behaviour;
  unsigned int slip;
  uint8_t texture;
  unsigned int rotation;
  float height[ 4 ];
  uint8_t shading[ 4 ];
} PmgTile;

typedef struct PmgChunk {
  PmgTile tiles[ 16 ];
} PmgChunk;

static PmgChunk mapChunks[ TERRAIN_CHUNKS ];

static const char *instructions =
	"Img2Pmg.exe myimage.png 100 myterrain.pmg";

static void LoadHeightmap( const char *path, int multiplier ) {
	PLImage *image = plLoadImage( path );
	if ( image == NULL ) {
		Error( "Failed to load the specified heightmap, \"%s\"!\nPL: %s\n", path, plGetError() );
	}

	static const unsigned int imageWidth = 65;
	if ( image->width < imageWidth || image->height < imageWidth ) {
		Error( "Invalid image size for heightmap, %dx%d vs 65x65!\n", image->width, image->height );
	}

	unsigned int channelLength = image->width * image->height;

	/* height */
	float *redChannel = malloc( sizeof( float ) * channelLength );
	uint8_t *pixel = image->data[ 0 ];
	for ( unsigned int i = 0; i < channelLength; ++i ) {
		redChannel[ i ] = (float) ( *pixel * multiplier );
		pixel += 4;
	}

	uint8_t *greenChannel = malloc( sizeof( uint8_t ) * channelLength );
	pixel = image->data[ 0 ] + 1;
	for ( unsigned int i = 0; i < channelLength; ++i ) {
		greenChannel[ i ] = *pixel;
		pixel += 4;
	}

	uint8_t *alphaChannel = malloc( sizeof( uint8_t ) * channelLength );
	pixel = image->data[ 0 ] + 3;
	for ( unsigned int i = 0; i < channelLength; ++i ) {
		alphaChannel[ i ] = *pixel;
		pixel += 4;
	}

	plDestroyImage( image );

	/* copy pasta this, since I can't be bothered writing it out again */
	for ( unsigned int chunk_y = 0; chunk_y < TERRAIN_CHUNK_ROW; ++chunk_y ) {
		for ( unsigned int chunk_x = 0; chunk_x < TERRAIN_CHUNK_ROW; ++chunk_x ) {
			PmgChunk *current_chunk = &mapChunks[ chunk_x + chunk_y * TERRAIN_CHUNK_ROW ];
			for ( unsigned int tile_y = 0; tile_y < TERRAIN_CHUNK_ROW_TILES; ++tile_y ) {
				for ( unsigned int tile_x = 0; tile_x < TERRAIN_CHUNK_ROW_TILES; ++tile_x ) {
					PmgTile *current_tile = &current_chunk->tiles[ tile_x + tile_y * TERRAIN_CHUNK_ROW_TILES ];
					unsigned int aaa = ( chunk_y * 4 * 65 ) + ( chunk_x * 4 );
					current_tile->height[ 0 ] = redChannel[ aaa + ( tile_y * imageWidth ) + tile_x ];
					current_tile->height[ 1 ] = redChannel[ aaa + ( tile_y * imageWidth ) + tile_x + 1 ];
					current_tile->height[ 2 ] = redChannel[ aaa + ( ( tile_y + 1 ) * imageWidth ) + tile_x ];
					current_tile->height[ 3 ] = redChannel[ aaa + ( ( tile_y + 1 ) * imageWidth ) + tile_x + 1 ];

					current_tile->texture = greenChannel[ aaa + ( tile_y * imageWidth ) + tile_x ];

					/* todo: perhaps use alpha channel? */
					current_tile->shading[ 0 ] =
					current_tile->shading[ 1 ] =
					current_tile->shading[ 2 ] =
					current_tile->shading[ 3 ] = 255; //alphaChannel[ aaa + ( tile_y * imageWidth ) + tile_x ];
				}
			}
		}
	}

	free( redChannel );
	free( greenChannel );
	free( alphaChannel );
}

static void WritePmg( const char *path ) {
	FILE *filePtr = fopen( path, "wb" );
	if ( filePtr == NULL ) {
		Error( "Failed to open \"%s\" for writing!\n" );
	}

	for ( unsigned int chunk_y = 0; chunk_y < TERRAIN_CHUNK_ROW; ++chunk_y ) {
		for ( unsigned int chunk_x = 0; chunk_x < TERRAIN_CHUNK_ROW; ++chunk_x ) {
			PmgChunk *current_chunk = &mapChunks[ chunk_x + chunk_y * TERRAIN_CHUNK_ROW ];

			struct __attribute__((packed)) {
			  /* offsets */
			  uint16_t x;
			  uint16_t y;
			  uint16_t z;
			  uint16_t unknown0;
			} chunk = {
				.x = 0,
				.y = 0,
				.z = 0,
				.unknown0 = 0,
			};

			fwrite( &chunk, sizeof( chunk ), 1, filePtr );

			struct __attribute__((packed)) {
			  int16_t height;
			  uint16_t lighting;
			} vertices[25];

			for ( unsigned int tile_y = 0; tile_y < TERRAIN_CHUNK_ROW_TILES; ++tile_y ) {
				for ( unsigned int tile_x = 0; tile_x < TERRAIN_CHUNK_ROW_TILES; ++tile_x ) {
					PmgTile *current_tile = &current_chunk->tiles[ tile_x + tile_y * TERRAIN_CHUNK_ROW_TILES ];

					vertices[ ( tile_y * 5 ) + tile_x ].height = current_tile->height[ 0 ];
					vertices[ ( tile_y * 5 ) + tile_x + 1 ].height = current_tile->height[ 1 ];
					vertices[ ( ( tile_y + 1 ) * 5 ) + tile_x ].height = current_tile->height[ 2 ];
					vertices[ ( ( tile_y + 1 ) * 5 ) + tile_x + 1 ].height = current_tile->height[ 3 ];
					vertices[ ( tile_y * 5 ) + tile_x ].lighting = current_tile->shading[ 0 ];
					vertices[ ( tile_y * 5 ) + tile_x + 1 ].lighting = current_tile->shading[ 0 ];
					vertices[ ( ( tile_y + 1 ) * 5 ) + tile_x ].lighting = current_tile->shading[ 0 ];
					vertices[ ( ( tile_y + 1 ) * 5 ) + tile_x + 1 ].lighting = current_tile->shading[ 0 ];
				}
			}

			fwrite( vertices, sizeof( *vertices ), 25, filePtr );

			uint32_t unknown = 0;
			fwrite( &unknown, sizeof( int32_t ), 1, filePtr );

			for ( unsigned int tile_y = 0; tile_y < TERRAIN_CHUNK_ROW_TILES; ++tile_y ) {
				for ( unsigned int tile_x = 0; tile_x < TERRAIN_CHUNK_ROW_TILES; ++tile_x ) {
					PmgTile *current_tile = &current_chunk->tiles[ tile_x + tile_y * TERRAIN_CHUNK_ROW_TILES ];

					struct __attribute__((packed)) {
					  int8_t unused0[6];
					  uint8_t type;
					  uint8_t slip;
					  int16_t unused1;
					  uint8_t rotation;
					  uint32_t texture;
					  uint8_t unused2;
					} tile = {
						.unused0 = { 0, 0, 0, 0, 0, 0 },
						.type = 0,
						.slip = 0,
						.unused1 = 0,
						.rotation = 0,
						.texture = current_tile->texture,
						.unused2 = 0
					};

					fwrite( &tile, sizeof( tile ), 1, filePtr );
				}
			}
		}
	}

	fclose( filePtr );
}

int main(int argc, char **argv) {
	plInitialize(argc, argv);

	plSetupLogOutput( "Img2Pmg.txt" );

	plSetupLogLevel(0, "info", PLColour(0, 255, 0, 255), true);
	plSetupLogLevel(1, "warning", PLColour(255, 255, 0, 255), true);
	plSetupLogLevel(2, "error", PLColour(255, 0, 0, 255), true);

	if(argc < 4) {
		printf("Invalid number of arguments!\n%s\n", instructions);
		return EXIT_SUCCESS;
	}

	char imgPath[PL_SYSTEM_MAX_PATH];
	strncpy(imgPath, argv[ 1 ], sizeof(imgPath));
	Print( "Input: %s\n", imgPath);
	char pmgPath[PL_SYSTEM_MAX_PATH];
	strncpy(pmgPath, argv[ 3 ], sizeof(pmgPath));
	Print( "Output: %s\n", pmgPath);

	int multiplier = atoi( argv[ 2 ] );

	LoadHeightmap( imgPath, multiplier );
	WritePmg( pmgPath );

	Print( "Done!\n");

	return EXIT_SUCCESS;
}
