/* OpenHoW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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

#include "pork_engine.h"

#include "client_video.h"
#include "client_frontend.h"
#include "client_shader.h"

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>

const char *video_paths[]={
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

#define MAX_QUEUED_VIDEOS   4

struct {
    bool is_playing;

    AVPacket        *av_packet;
    AVCodecContext  *av_codec_context;
    AVCodec         *av_codec;
    AVStream        *av_stream;
    AVFormatContext *av_format_context;
    AVFrame         *av_frame;

    unsigned int stream_index;

    struct {
        char path[PL_SYSTEM_MAX_PATH];
    } queue[MAX_QUEUED_VIDEOS];
    unsigned int num_videos_queued; /* elements */
    unsigned int cur_video;         /* index */
} video;

void PlayVideoCommand(unsigned int argc, char *argv[]) {
    if(argc < 1) {
        LogWarn("invalid number of arguments, ignoring!\n"
                "use \"play_video /streams/sheff.bik /streams/infologo.bik\", for example.\n");
        return;
    }

    const char *cmd = argv[1];
    void QueueVideos(const char **videos, unsigned int num_videos);
    QueueVideos(&cmd, argc - 1);

    /* immediately begin playing the videos */
    SetFrontendState(FE_MODE_VIDEO);
}

void InitVideo(void) {
    memset(&video, 0, sizeof(video));

    av_register_all();
    avformat_network_init();

    plRegisterConsoleCommand("play_video", PlayVideoCommand, "Play the given videos.");
    plParseConsoleString("play_video \"streams/fmv 01.bik\"");
}

void ShutdownVideo(void) {
    ClearVideoQueue();
}

/***************************************************************/

void ClearVideoQueue(void) {
    memset(video.queue, 0, sizeof(video.queue) * MAX_QUEUED_VIDEOS);
    video.cur_video = 0;
    video.num_videos_queued = 0;
}

void QueueVideos(const char **videos, unsigned int num_videos) {
    if(num_videos == 0) {
        return;
    }

    ClearVideoQueue();

    for(unsigned int i = 0; i < num_videos; ++i) {
        if(plIsEmptyString(videos[i])) {
            LogWarn("encountered invalid video path at index %u, skipping!\n", i);
            continue;
        }

        size_t len = strlen(videos[i]);
        if(len >= PL_SYSTEM_MAX_PATH) {
            LogWarn("unexpected length of path - %u bytes - expect issues!\n", len);
        }

        char n_path[PL_SYSTEM_MAX_PATH];
        strncpy(n_path, videos[i], PL_SYSTEM_MAX_PATH);
        if(!plFileExists(n_path)) {
            LogWarn("failed to find video at \"%s\", skipping!\n", n_path);
            continue;
        }

        strncpy(video.queue[video.num_videos_queued].path, n_path, PL_SYSTEM_MAX_PATH);
        video.num_videos_queued++;
    }
}

void PlayVideo(const char *path) {
    ClearVideoQueue();
    QueueVideos(&path, 1);
    if(video.num_videos_queued == 0) {
        LogWarn("failed to queue video, \"%s\", aborting playback!\n", path);
        return;
    }

    /* immediately begin playing the videos */
    SetFrontendState(FE_MODE_VIDEO);
}

void SkipVideo(void) {
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
            SkipVideo();
            return;
        }
        return;
    }

    /* nothing left to play */
    FE_RestoreLastState();
}

void ProcessVideo(void) {
#if 0
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
#endif
}

void DrawVideo(void) {
    static PLMesh *mesh = NULL;
    if(mesh == NULL) {
        if((mesh = plCreateMesh(PL_MESH_TRIANGLE_STRIP, PL_DRAW_STATIC, 2, 4)) == NULL) {
            Error("failed to create mesh for video renderer, aborting!\n%s\n", plGetError());
        }

        plClearMesh(mesh);

#define x 0
#define y 0
#define w 640
#define h 480

        plSetMeshVertexPosition(mesh, 0, PLVector3(x, y, 0));
        plSetMeshVertexPosition(mesh, 1, PLVector3(x, y + h, 0));
        plSetMeshVertexPosition(mesh, 2, PLVector3(x + w, y, 0));
        plSetMeshVertexPosition(mesh, 3, PLVector3(x + w, y + h, 0));

        plSetMeshUniformColour(mesh, PLColourRGB(255, 255, 255));

        plSetMeshVertexST(mesh, 0, 0, 0);
        plSetMeshVertexST(mesh, 1, 0, 1);
        plSetMeshVertexST(mesh, 2, 1, 0);
        plSetMeshVertexST(mesh, 3, 1, 1);

        plUploadMesh(mesh);
    }

    plSetShaderProgram(programs[SHADER_VIDEO]);

    /* todo pass correct texture */

    plDrawMesh(mesh);

    plSetShaderProgram(programs[SHADER_DEFAULT]);
}
