/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
 * Copyright (C) 2017 Daniel Collins <solemnwarning@solemnwarning.net>
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

#include "extractor.h"
#include "fac.h"

static char g_input_path[ PL_SYSTEM_MAX_PATH ] = { '\0' };
static char g_output_path[ PL_SYSTEM_MAX_PATH ];

//#define PARANOID_DATA
//#define EXPORT_NORMALS
#define CONVERT_TIMS

/************************************************************/
/* Data Conversion */

static void ConvertImageToPng( const char *path ) {
#if defined( CONVERT_TIMS )
	// figure out if the file already exists before
	// we even start trying to convert this thing
	char out_path[ PL_SYSTEM_MAX_PATH ];
	PlStripExtension( out_path, sizeof( out_path ), path );
	strcat( out_path, ".png" );
	if ( PlFileExists( out_path ) ) {
		PlDeleteFile( path );
		return;
	}

	PLImage *image = PlLoadImage( path );
	if ( image == NULL ) {
		Warning( "Failed to load image, \"%s\" (%s)!\n", path, PlGetError() );
		return;
	}

	const char *ext = PlGetFileExtension( path );
	if ( ext != NULL && ext[ 0 ] != '\0' && strcmp( ext, "tim" ) == 0 ) {
		// ensure that it's a format we're able to convert from
		if ( image->format == PL_IMAGEFORMAT_RGB5A1 ) {
			if ( PlConvertPixelFormat( image, PL_IMAGEFORMAT_RGBA8 ) ) {
				PlReplaceImageColour( image, PLColour( 255, 0, 255, 255 ), PLColour( 0, 0, 0, 0 ) );
				if ( !PlWriteImage( image, out_path ) ) {
					Warning( "Failed to write PNG, \"%s\" (%s)!\n", out_path, PlGetError() );
				}
			} else {
				Warning( "Failed to convert \"%s\", %s, aborting!\n", path, PlGetError() );
			}
		} else {
			Warning( "Unexpected pixel format in \"%s\", aborting!\n", path );
		}
	}

	if ( PlFileExists( out_path ) ) {
		PlDeleteFile( path );
	}

	PlDestroyImage( image );
#endif
}

typedef struct ModelConversionData {
	const char *mad;
	const char *mtd;
	const char *out;
} ModelConversionData;
static ModelConversionData pc_conversion_data[] = {
        { "/Maps/BAY.MAD", "/Maps/bay.mtd", "mods/how/chars/scenery/" },
        { "/Maps/ICE.MAD", "/Maps/ice.mtd", "mods/how/chars/scenery/" },
        { "/Maps/BOOM.MAD", "/Maps/boom.mtd", "mods/how/chars/scenery/" },
        { "/Maps/BUTE.MAD", "/Maps/bute.mtd", "mods/how/chars/scenery/" },
        { "/Maps/CAMP.MAD", "/Maps/camp.mtd", "mods/how/chars/scenery/" },
        { "/Maps/DEMO.MAD", "/Maps/demo.mtd", "mods/how/chars/scenery/" },
        { "/Maps/DEVI.MAD", "/Maps/devi.mtd", "mods/how/chars/scenery/" },
        { "/Maps/DVAL.MAD", "/Maps/dval.mtd", "mods/how/chars/scenery/" },
        { "/Maps/EASY.MAD", "/Maps/easy.mtd", "mods/how/chars/scenery/" },
        { "/Maps/ESTU.MAD", "/Maps/estu.mtd", "mods/how/chars/scenery/" },
        { "/Maps/FOOT.MAD", "/Maps/foot.mtd", "mods/how/chars/scenery/" },
        { "/Maps/GUNS.MAD", "/Maps/guns.mtd", "mods/how/chars/scenery/" },
        { "/Maps/HELL2.MAD", "/Maps/hell2.mtd", "mods/how/chars/scenery/" },
        { "/Maps/HELL3.MAD", "/Maps/hell3.mtd", "mods/how/chars/scenery/" },
        { "/Maps/HILLBASE.MAD", "/Maps/hillbase.mtd", "mods/how/chars/scenery/" },
        { "/Maps/ICEFLOW.MAD", "/Maps/iceflow.mtd", "mods/how/chars/scenery/" },
        { "/Maps/ICE.MAD", "/Maps/ice.mtd", "mods/how/chars/scenery/" },
        { "/Maps/ZULUS.MAD", "/Maps/zulus.mtd", "mods/how/chars/scenery/" },

        { "/Chars/WEAPONS.MAD", "/Chars/WEAPONS.MTD", "mods/how/chars/weapons/" },
        { "/Chars/PROPOINT.MAD", "/Chars/propoint.mtd", "mods/how/chars/propoint/" },
        { "/Chars/TOP.MAD", "/Chars/TOP.MTD", "mods/how/chars/top/" },
        { "/Chars/SIGHT.MAD", "/Chars/SIGHT.MTD", "mods/how/chars/sight/" },

        { "/Chars/british.mad", "/Chars/british.mtd", "mods/how/chars/pigs/" }, /* pig models */

        { "/Chars/SKYDOME.MAD", "/Chars/SKYDOME.MTD", "mods/how/skys/" },
};

static void ConvertModelData( void ) {
	for ( unsigned long i = 0; i < PL_ARRAY_ELEMENTS( pc_conversion_data ); ++i ) {
		char path[ PL_SYSTEM_MAX_PATH ];
		snprintf( path, sizeof( path ), "%s%s", g_input_path, pc_conversion_data[ i ].mad );
		PLPackage *package = PlLoadPackage( path );
		if ( package == NULL ) {
			Error( "Failed to load MAD package, \"%s\" (%s)!\n", path, PlGetError() );
		}

		/* Write the files out to the destination. I'm lazy and we'll delete it once we're done anyway. */
		char model_paths[ package->table_size ][ PL_SYSTEM_MAX_PATH ];
		unsigned int num_models = 0;
		for ( unsigned int j = 0; j < package->table_size; ++j ) {
			char out[ PL_SYSTEM_MAX_PATH ];
			snprintf( out, sizeof( out ), "%s/%s%s", g_output_path, pc_conversion_data[ i ].out, pl_strtolower( package->table[ j ].fileName ) );

			char dir[ PL_SYSTEM_MAX_PATH ];
			const char *filename = PlGetFileName( out );
			strncpy( dir, out, strlen( out ) - strlen( filename ) );
			dir[ strlen( out ) - strlen( filename ) ] = '\0';
			if ( PlCreatePath( dir ) ) {
				PLFile *fp = PlLoadPackageFile( package, package->table[ j ].fileName );
				if ( !PlWriteFile( out, PlGetFileData( fp ), package->table[ j ].fileSize ) ) {
					Error( "Failed to write model, \"%s\" (%s)!\n", out, PlGetError() );
				}
				PlCloseFile( fp );
			} else {
				Error( "Failed to create output directory, \"%s\" (%s)!\n", dir, PlGetError() );
			}

			// skydome is a special case
			if ( pl_strcasecmp( filename, "skydomeu.fac" ) == 0 || pl_strcasecmp( filename, "skydome.fac" ) == 0 ) {
				continue;
			}

			const char *ext = PlGetFileExtension( package->table[ j ].fileName );
			if ( pl_strcasecmp( ext, "fac" ) == 0 ) {
				PlStripExtension( model_paths[ num_models++ ], PL_SYSTEM_MAX_PATH - 1, out );
			}
		}

		PlDestroyPackage( package );

		/* Now we need to load each fac, fetch each index for each texture and figure out
		 * the true name for that texture by comparing against the mtd. */

		snprintf( path, sizeof( path ), "%s%s", g_input_path, pc_conversion_data[ i ].mtd );
		package = PlLoadPackage( path );
		if ( package == NULL ) {
			Error( "Failed to load MTD package, \"%s\" (%s)!\n", pc_conversion_data[ i ].mtd, PlGetError() );
		}

		/* pigs are a special case... */
		if ( strcmp( pc_conversion_data[ i ].mad, "/Chars/british.mad" ) != 0 ) {
			// write all the textures out
			for ( unsigned int j = 0; j < package->table_size; ++j ) {
				char out[ PL_SYSTEM_MAX_PATH ];
				snprintf( out, sizeof( out ), "%s/%s%s", g_output_path, pc_conversion_data[ i ].out, pl_strtolower( package->table[ j ].fileName ) );

				char dir[ PL_SYSTEM_MAX_PATH ];
				const char *filename = PlGetFileName( out );
				strncpy( dir, out, strlen( out ) - strlen( filename ) );
				dir[ strlen( out ) - strlen( filename ) ] = '\0';

#if defined( PARANOID_DATA )// paranoid check, doesn't happen...
				if ( plFileExists( out ) ) {
					LogInfo( "The file \"%s\" already exists, comparing...\n", out );

					size_t size = plGetLocalFileSize( out );
					FILE *fp = fopen( out, "rb" );
					char *data = u_alloc( size, 1, true );
					fread( data, 1, size, fp );
					fclose( fp );

					uint32_t crc_a;
					pl_crc32( package->table[ j ].file.data, package->table[ j ].fileSize, &crc_a );
					uint32_t crc_b;
					pl_crc32( data, size, &crc_b );
					if ( crc_a != crc_b ) {
						LogWarn( "Files are different, renaming second file!\n" );
						while ( plFileExists( out ) ) {
							strcat( out, "_" );
						}
					}
				}
#endif

				if ( PlCreatePath( dir ) ) {
					PLFile *fp = PlLoadPackageFile( package, package->table[ j ].fileName );
					if ( !PlWriteFile( out, PlGetFileData( fp ), package->table[ j ].fileSize ) ) {
						Error( "Failed to write model, \"%s\" (%s)!\n", out, PlGetError() );
					}
					PlCloseFile( fp );
				} else {
					Error( "Failed to create output directory, \"%s\" (%s)!\n", dir, PlGetError() );
				}

				ConvertImageToPng( out );
			}
		}

		/* and now we go through again, converting everything as we do so */
		for ( unsigned int j = 0; j < num_models; ++j ) {
			char fac_path[ PL_SYSTEM_MAX_PATH ];
			snprintf( fac_path, PL_SYSTEM_MAX_PATH, "%s.fac", model_paths[ j ] );
			if ( !PlFileExists( fac_path ) ) {
				Error( "Failed to find FAC file, \"%s\"!\n", fac_path );
			}

			// we'll resize this later...
			FacTextureIndex *table = calloc( package->table_size, sizeof( FacTextureIndex ) );
			if ( table == NULL ) {
				Warning( "Failed to allocate texture table!\n" );
				continue;
			}

			unsigned int table_size = 0;

			FacHandle *fac = Fac_LoadFile( fac_path );
			if ( fac == NULL ) {
				Warning( "Failed to load FAC \"%s\"!\n", fac_path );
				continue;
			}

			for ( unsigned int k = 0; k < fac->num_triangles; ++k ) {
				uint32_t texture_index = fac->triangles[ k ].texture_index;
				if ( texture_index >= package->table_size ) {
					Error( "Out of bounds texture index, \"%s\"!\n", fac_path );
				}

				// attempt to add it to the table
				char texture_name[ 16 ];
				strncpy( texture_name, package->table[ texture_index ].fileName, strlen( package->table[ texture_index ].fileName ) - 4 );
				texture_name[ strlen( package->table[ texture_index ].fileName ) - 4 ] = '\0';
				pl_strtolower( texture_name );
				unsigned int l;
				for ( l = 0; l < package->table_size; ++l ) {
					if ( table[ l ].name[ 0 ] == '\0' ) {
						strncpy( table[ l ].name, texture_name, sizeof( table[ l ].name ) );
						table_size++;
						break;
					} else if ( strncmp( table[ l ].name, texture_name, sizeof( table[ l ].name ) ) == 0 ) {
						break;
					}
				}

				if ( table_size > package->table_size ) {
					Error( "Invalid table size, %d > %d!\n", table_size, package->table_size );
				}

				// replace the original id so it matches with the index in our table
				fac->triangles[ k ].texture_index = l;
			}

			fac->texture_table = calloc( table_size, sizeof( FacTextureIndex ) );
			if ( fac->texture_table == NULL ) {
				Error( "Failed to allocate texture table for output!\n" );
			}

			fac->texture_table_size = table_size;
			memcpy( fac->texture_table, table, sizeof( FacTextureIndex ) * table_size );
			free( table );

			// write out the fac and replace it (we'll append the table to the end)
			snprintf( fac_path, PL_SYSTEM_MAX_PATH, "%s.fac", model_paths[ j ] );
			Fac_WriteFile( fac, fac_path );
		}
	}
}

/////////////////////////////////////////////////////////////
/* Extraction process for initial setup */

static void ExtractPtgPackage( const char *input_path, const char *output_path ) {
	if ( !PlCreatePath( output_path ) ) {
		Error( "Failed to create path, \"%s\" (%s)!\n", output_path, PlGetError() );
	}

	FILE *file = fopen( input_path, "rb" );
	if ( file == NULL ) {
		Error( "Failed to load PTG package, \"%s\"!\n", input_path );
	}

	uint32_t num_textures = 0;
	if ( fread( &num_textures, sizeof( uint32_t ), 1, file ) != 1 ) {
		Error( "Invalid PTG file, failed to get number of textures!\n" );
	}

	size_t tim_size = ( PlGetLocalFileSize( input_path ) - sizeof( num_textures ) ) / num_textures;
	for ( unsigned int i = 0; i < num_textures; ++i ) {
		uint8_t tim[ tim_size ];
		if ( fread( tim, tim_size, 1, file ) != 1 ) {
			Print( "Failed to read Tim, \"%d\"!\n", i );
			continue;
		}

		char out_path[ PL_SYSTEM_MAX_PATH ] = { '\0' };
		sprintf( out_path, "%s%d.tim", output_path, i );
		if ( !PlWriteFile( out_path, tim, tim_size ) ) {
			Warning( "Failed to write file, \"%s\" (%s)!\n", out_path, PlGetError() );
		}

		ConvertImageToPng( out_path );
	}

	fclose( file );
}

static void ExtractMadPackage( const char *input_path, const char *output_path ) {
	if ( !PlCreatePath( output_path ) ) {
		Error( "Failed to create output directory,  \"%s\"!\nPL: %s\n", output_path, PlGetError() );
	}

	PLPackage *package = PlLoadPackage( input_path );
	if ( package == NULL ) {
		Error( "Failed to load %s, aborting!\nPL: %s\n", input_path, PlGetError() );
	}

	for ( unsigned int i = 0; i < package->table_size; i++ ) {
		char out[ PL_SYSTEM_MAX_PATH ];
		snprintf( out, sizeof( out ) - 1, "%s%s", output_path, pl_strtolower( package->table[ i ].fileName ) );
		PLFile *fp = PlLoadPackageFile( package, package->table[ i ].fileName );
		if ( !PlWriteFile( out, PlGetFileData( fp ), package->table[ i ].fileSize ) ) {
			Error( "Failed to write file, \"%s\" (%s)!\n", out, PlGetError() );
		}
		PlCloseFile( fp );

		const char *ext = PlGetFileExtension( out );
		if ( strcmp( ext, "tim" ) == 0 ) {
			ConvertImageToPng( out );
		}
	}

	PlDestroyPackage( package );
}

/************************************************************/
/* Texture Merger */

typedef struct TextureMerge {
	const char *output;
	unsigned int num_textures;
	unsigned int width, height;
	struct {
		const char *path;
		unsigned int x, y;
	} targets[ 10 ];
} TextureMerge;
static TextureMerge texture_targets[] = {
        { "/mods/how/frontend/dash/ang.png", 5, 152, 121, { { "/mods/how/frontend/dash/ang1.png", 0, 0 }, { "/mods/how/frontend/dash/ang2.png", 64, 22 }, { "/mods/how/frontend/dash/ang3.png", 0, 64 }, { "/mods/how/frontend/dash/ang4.png", 64, 64 }, { "/mods/how/frontend/dash/ang5.png", 128, 31 } } },

        { "/mods/how/frontend/dash/clock.png", 4, 128, 96, { { "/mods/how/frontend/dash/clock01.png", 0, 0 }, { "/mods/how/frontend/dash/clock02.png", 64, 0 }, { "/mods/how/frontend/dash/clock03.png", 0, 28 }, { "/mods/how/frontend/dash/clock04.png", 64, 28 } } },

        { "/mods/how/frontend/dash/timer.png", 10, 256, 32, { { "/mods/how/frontend/dash/timer0.png", 0, 0 }, { "/mods/how/frontend/dash/timer1.png", 24, 0 }, { "/mods/how/frontend/dash/timer2.png", 48, 0 }, { "/mods/how/frontend/dash/timer3.png", 72, 0 }, { "/mods/how/frontend/dash/timer4.png", 96, 0 }, { "/mods/how/frontend/dash/timer5.png", 120, 0 }, { "/mods/how/frontend/dash/timer6.png", 144, 0 }, { "/mods/how/frontend/dash/timer7.png", 168, 0 }, { "/mods/how/frontend/dash/timer8.png", 192, 0 }, { "/mods/how/frontend/dash/timer9.png", 216, 0 } } },

        { "/mods/how/frontend/dash/pause.png", 8, 48, 48, { { "/mods/how/frontend/dash/pause1.png", 0, 0 }, { "/mods/how/frontend/dash/pause2.png", 16, 0 }, { "/mods/how/frontend/dash/pause3.png", 32, 0 }, { "/mods/how/frontend/dash/pause4.png", 0, 16 }, { "/mods/how/frontend/dash/pause5.png", 32, 16 }, { "/mods/how/frontend/dash/pause6.png", 0, 32 }, { "/mods/how/frontend/dash/pause7.png", 16, 32 }, { "/mods/how/frontend/dash/pause8.png", 32, 32 } } } };

static void MergeTextureTargets( void ) {
	unsigned int num_texture_targets = PL_ARRAY_ELEMENTS( texture_targets );
	Print( "Merging %d texture targets...\n", num_texture_targets );
	for ( unsigned int i = 0; i < num_texture_targets; ++i ) {
		TextureMerge *merge = &texture_targets[ i ];
		Print( "Generating %s\n", merge->output );
		PLImage *output = PlCreateImage( NULL, merge->width, merge->height, PL_COLOURFORMAT_RGBA, PL_IMAGEFORMAT_RGBA8 );
		if ( output == NULL ) {
			Warning( "Failed to generate texture target (%s)!\n", PlGetError() );
			continue;
		}

		for ( unsigned int j = 0; j < merge->num_textures; ++j ) {
			const char *path = merge->targets[ j ].path;
			PLImage *image = PlLoadImage( path );
			if ( image == NULL ) {
				Warning( "Failed to find image, \"%s\", for merge!\n", merge->targets[ j ].path );
				continue;
			}

			Print( "Writing %s into %s\n", merge->targets[ j ].path, merge->output );

			uint8_t
			        *pos = output->data[ 0 ] + ( ( merge->targets[ j ].y * output->width ) + merge->targets[ j ].x ) * 4;
			uint8_t *src = image->data[ 0 ];
			for ( unsigned int y = 0; y < image->height; ++y ) {
				memcpy( pos, src, ( image->width * 4 ) );
				src += image->width * 4;
				pos += output->width * 4;
			}

			PlDestroyImage( image );
			PlDeleteFile( path );
		}

		Print( "Writing %s\n", merge->output );
		PlWriteImage( output, merge->output );
		PlDestroyImage( output );
	}
}

/************************************************************/

typedef struct IOPath {
	const char *input, *output;
} IOPath;

static IOPath pc_music_paths[] = {
#include "pc_music_paths.h"
};

static IOPath pc_copy_paths[] = {
#include "pc_copy_paths.h"
};

static IOPath pc_package_paths[] = {
#include "pc_package_paths.h"
};

static void ProcessPackagePaths( const char *in, const char *out, const IOPath *paths, unsigned int length ) {
	for ( unsigned int i = 0; i < length; ++i ) {
		char output_path[ PL_SYSTEM_MAX_PATH ];
		snprintf( output_path, sizeof( output_path ), "%s%s", out, paths[ i ].output );
		if ( !PlCreatePath( output_path ) ) {
			Warning( "%s\n", PlGetError() );
			continue;
		}

		char input_path[ PL_SYSTEM_MAX_PATH ];
		snprintf( input_path, sizeof( input_path ), "%s%s", in, paths[ i ].input );
		Print( "Copying %s to %s\n", input_path, output_path );
		const char *ext = PlGetFileExtension( input_path );
		if ( pl_strncasecmp( ext, "PTG", 3 ) == 0 ) {
			ExtractPtgPackage( input_path, output_path );
		} else {
			ExtractMadPackage( input_path, output_path );
		}
	}
}

static void ProcessCopyPaths( const char *in, const char *out, const IOPath *paths, unsigned int length ) {
	for ( unsigned int i = 0; i < length; ++i ) {
		char output_path[ PL_SYSTEM_MAX_PATH ];
		snprintf( output_path, sizeof( output_path ), "%s%s", out, paths[ i ].output );

		// Fudge the path if it's one of the audio tracks
		char *p = strstr( output_path, "sku1/" );
		if ( p != NULL ) {
			strncpy( p, region_idents[ version_info.region ], 3 );
			memmove( p + 3, p + 4, strlen( p + 4 ) + 1 );
		}

		if ( !PlCreatePath( output_path ) ) {
			Error( "Failed to create path, \"%s\" (%s)!\n", output_path, PlGetError() );
		}

		strncat( output_path, PlGetFileName( paths[ i ].input ), sizeof( output_path ) - strlen( output_path ) - 1 );
		pl_strtolower( output_path );

		char input_path[ PL_SYSTEM_MAX_PATH ];
		snprintf( input_path, sizeof( input_path ), "%s%s", in, paths[ i ].input );
		Print( "Copying %s to %s\n", input_path, output_path );
		PlCopyFile( input_path, output_path );
	}
}

unsigned int MSG_LVL_PRINT;
unsigned int MSG_LVL_WARN;
unsigned int MSG_LVL_ERR;

int main( int argc, char **argv ) {
	if ( argc == 1 ) {
		printf( "Invalid number of arguments ...\n"
		        "  extractor <game_path> -<out_path>\n" );
		return EXIT_SUCCESS;
	}

	PlInitialize( argc, argv );

	PlRegisterStandardPackageLoaders();
	PlRegisterStandardImageLoaders( PL_IMAGE_FILEFORMAT_TIM | PL_IMAGE_FILEFORMAT_BMP | PL_IMAGE_FILEFORMAT_PNG );

	char app_dir[ PL_SYSTEM_MAX_PATH ];
	PlGetApplicationDataDirectory( "OpenHoW", app_dir, PL_SYSTEM_MAX_PATH );
	if ( !PlCreatePath( app_dir ) ) {
		Warning( "Unable to create %s: %s\nSettings will not be saved.", app_dir, PlGetError() );
	}

	char log_path[ PL_SYSTEM_MAX_PATH ];
	snprintf( log_path, PL_SYSTEM_MAX_PATH, "%s/extractor", app_dir );
	PlSetupLogOutput( log_path );

	MSG_LVL_PRINT = PlAddLogLevel( "print", PL_COLOUR_GREEN, true );
	MSG_LVL_WARN = PlAddLogLevel( "warning", PL_COLOUR_YELLOW, true );
	MSG_LVL_ERR = PlAddLogLevel( "error", PL_COLOUR_RED, true );

	/* now deal with any arguments */

#if 0
	if(getcwd(output_path, sizeof(output_path) - 1) == NULL) {
		strcpy(output_path, "./");
	}
#else
	strcpy( g_output_path, "./" );
#endif

	for ( int i = 1; i < argc; ++i ) {
		if ( argv[ i ][ 0 ] == '-' ) {
			strncpy( g_output_path, argv[ i ] + 1, sizeof( g_output_path ) );
		} else {
			strncpy( g_input_path, argv[ i ], sizeof( g_input_path ) );
		}
	}

	if ( *g_input_path == '\0' ) {
		Error( "Empty game path, aborting!\n" );
	}

	Print( "\n"
	       "output path: %s\n"
	       "input path:  %s\n",
	       g_output_path, g_input_path );

	/* ensure it's a valid version - original CD version requires
	 * us to extract audio from the CD while GOG and Steam versions
	 * provide OGG audio.
	 *
	 * PSX is a totally different story, with it's own fun adventure. */
	CheckGameVersion( g_input_path );
	if ( version_info.region == REGION_UNKNOWN || version_info.platform == PLATFORM_UNKNOWN ) {
		Error( "Unknown game version, aborting!\n" );
	} else if ( version_info.platform == PLATFORM_PSX ) {
		Error( "Unsupported platform!\n" );
	}

	if ( version_info.platform == PLATFORM_PC || version_info.platform == PLATFORM_PC_DIGITAL ) {
		ProcessPackagePaths( g_input_path, g_output_path, pc_package_paths, PL_ARRAY_ELEMENTS( pc_package_paths ) );
		ProcessCopyPaths( g_input_path, g_output_path, pc_copy_paths, PL_ARRAY_ELEMENTS( pc_copy_paths ) );

		if ( version_info.platform == PLATFORM_PC_DIGITAL ) {
			// They've done us the honors for the digital version
			ProcessCopyPaths( g_input_path, g_output_path, pc_music_paths, PL_ARRAY_ELEMENTS( pc_music_paths ) );
		} else {
			// todo: rip the disc...
		}
	}

	MergeTextureTargets();

	ConvertModelData();

	Print( "Complete!\n" );
	return EXIT_SUCCESS;
}
