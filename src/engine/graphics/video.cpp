// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Menu.h"
#include "video.h"
#include "ShaderManager.h"

#if 0
#	include <libavformat/avformat.h>
#	include <libavcodec/avcodec.h>
#	include <libavfilter/avfilter.h>
#	include <libavdevice/avdevice.h>
#endif

const char *video_paths[] = {
        /* intro */
        "/streams/sheff.bik",
        "/streams/infologo.bik",

        /* missions - move these into script */
        "/streams/fmv 01.bik",
        "/streams/fmv 02.bik",
        "/streams/fmv 03.bik",
        "/streams/fmv 04.bik",
        "/streams/fmv 05.bik",
        "/streams/fmv 06.bik",
        "/streams/fmv 07.bik",
        "/streams/fmv 08.bik",
        "/streams/fmv 09.bik",
};

#define MAX_QUEUED_VIDEOS 4

typedef struct VideoState {
	bool is_playing;

#if 0
	AVPacket        *av_packet;
	AVCodecContext  *av_codec_context;
	AVCodec         *av_codec;
	AVStream        *av_stream;
	AVFormatContext *av_format_context;
	AVFrame         *av_frame;
#endif

	unsigned int stream_index;

	struct {
		char path[ PL_SYSTEM_MAX_PATH ];
	} queue[ MAX_QUEUED_VIDEOS ];
	unsigned int num_videos_queued; /* elements */
	unsigned int cur_video;         /* index */
} VideoState;
static VideoState video;

void PlayVideoCommand( unsigned int argc, char *argv[] ) {
	if ( argc < 1 ) {
		Warning( "invalid number of arguments, ignoring!\n"
		         "use \"play_video /streams/sheff.bik /streams/infologo.bik\", for example.\n" );
		return;
	}

	const char *cmd = argv[ 1 ];
	Video_QueuePlayback( &cmd, argc - 1 );

	/* immediately begin playing the videos */
	FrontEnd_SetState( FE_MODE_VIDEO );
}

void Video_Initialize( void ) {
	memset( &video, 0, sizeof( VideoState ) );

#if 0
	av_register_all();
	avformat_network_init();
#endif

	PlRegisterConsoleCommand( "play_video", "Play the given videos.", 1, PlayVideoCommand );
	PlParseConsoleString( "play_video \"streams/fmv 01.bik\"" );
}

void Video_Shutdown( void ) {
	Video_ClearQueue();
}

/************************************************************/

void Video_ClearQueue( void ) {
	memset( video.queue, 0, sizeof( video.queue ) );
	video.cur_video = 0;
	video.num_videos_queued = 0;
}

void Video_QueuePlayback( const char **videos, unsigned int num_videos ) {
	if ( num_videos == 0 ) {
		return;
	}

	Video_ClearQueue();

	for ( unsigned int i = 0; i < num_videos; ++i ) {
		if ( videos[ i ] == nullptr ) {
			Warning( "encountered invalid video path at index %u, skipping!\n", i );
			continue;
		}

		size_t len = strlen( videos[ i ] );
		if ( len >= PL_SYSTEM_MAX_PATH ) {
			Warning( "unexpected length of path - %u bytes - expect issues!\n", len );
		}

		char n_path[ PL_SYSTEM_MAX_PATH ];
		strncpy( n_path, videos[ i ], PL_SYSTEM_MAX_PATH );
		if ( !PlFileExists( n_path ) ) {
			Warning( "failed to find video at \"%s\", skipping!\n", n_path );
			continue;
		}

		strncpy( video.queue[ video.num_videos_queued ].path, n_path, PL_SYSTEM_MAX_PATH );
		video.num_videos_queued++;
	}
}

void Video_Play( const char *path ) {
	Video_ClearQueue();
	Video_QueuePlayback( &path, 1 );
	if ( video.num_videos_queued == 0 ) {
		Warning( "failed to queue video, \"%s\", aborting playback!\n", path );
		return;
	}

	/* immediately begin playing the videos */
	FrontEnd_SetState( FE_MODE_VIDEO );
}

void Video_SkipCurrent( void ) {
#if 0
	if(video.av_packet != NULL) {
		av_packet_unref(video.av_packet);
		video.av_packet = NULL;
	}

	if(video.av_format_context != NULL) {
		/* automatically sets av_format_context to null */
		avformat_close_input(&video.av_format_context);
	}

	if(video.num_videos_queued > 0 && video.cur_video != video.num_videos_queued) {
		if(avformat_open_input(&video.av_format_context, video.queue[video.cur_video].path, NULL, NULL) < 0) {
			LogWarn("failed to open video, \"%s\", for playback, skipping!\n");
			Video_SkipCurrent();
			return;
		}
		return;
	}
#endif

	/* nothing left to play */
	FE_RestoreLastState();
}

#if 0
static void ProcessVideo(void) {

	do {
		/* decode the video */

		if(av_read_frame(video.av_format_context, video.av_packet) < 0) {
			LogWarn("failed to read video frame!\n");
			SkipVideo();
			return;
		}

		if(video.av_packet->stream_index == video.stream_index) {
			if(avcodec_send_packet(video.av_codec_context, video.av_packet) < 0) {
				LogWarn("failed to send video packet!\n");
				SkipVideo();
				return;
			}

			int val = avcodec_receive_frame(video.av_codec_context, video.av_frame);
			if(val < 0 && val != AVERROR(EAGAIN) && val != AVERROR_EOF) {
				LogWarn("failed to receive video frame!\n");
				SkipVideo();
				return;
			}

			/* todo: stuff... */

			av_frame_unref(video.av_frame);
		}

		av_packet_unref(video.av_packet);

		/* and now the audio ... */


	} while(video.av_packet->stream_index != video.stream_index);
}
#endif

void Video_Draw( void ) {
	static PLGMesh *mesh = NULL;
	if ( mesh == NULL ) {
		if ( ( mesh = PlgCreateMesh( PLG_MESH_TRIANGLE_STRIP, PLG_DRAW_STATIC, 2, 4 ) ) == NULL ) {
			Error( "failed to create mesh for video renderer, aborting!\n%s\n", PlGetError() );
		}

		PlgClearMesh( mesh );

#define x 0
#define y 0
#define w 640
#define h 480

		PlgSetMeshVertexPosition( mesh, 0, hei::Vector3( x, y, 0 ) );
		PlgSetMeshVertexPosition( mesh, 1, hei::Vector3( x, y + h, 0 ) );
		PlgSetMeshVertexPosition( mesh, 2, hei::Vector3( x + w, y, 0 ) );
		PlgSetMeshVertexPosition( mesh, 3, hei::Vector3( x + w, y + h, 0 ) );

		PlgSetMeshUniformColour( mesh, PLColourRGB( 255, 255, 255 ) );

		PlgSetMeshVertexST( mesh, 0, 0, 0 );
		PlgSetMeshVertexST( mesh, 1, 0, 1 );
		PlgSetMeshVertexST( mesh, 2, 1, 0 );
		PlgSetMeshVertexST( mesh, 3, 1, 1 );

		PlgUploadMesh( mesh );
	}

	Shaders_SetProgramByName( "generic_textured" );

	PLGShaderProgram *program = PlgGetCurrentShaderProgram();
	if ( program == nullptr ) {
		return;
	}

	/* todo pass correct texture */
	PLMatrix4 matrix = PlMatrix4Identity();
	PlgSetShaderUniformValue( program, "pl_model", &matrix, false );
	PlgDrawMesh( mesh );
}
