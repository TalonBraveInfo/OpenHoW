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
#include "pork_engine.h"

#include "client_frontend.h"

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>

const char *video_paths[]={
        /* intro */
        "/streams/sheff.bik",
        "/streams/infologo.bik",

        /* missions - move these into js script */
        "/streams/fmv_01.bik",
        "/streams/fmv_02.bik",
        "/streams/fmv_03.bik",
        "/streams/fmv_04.bik",
        "/streams/fmv_05.bik",
        "/streams/fmv_06.bik",
        "/streams/fmv_07.bik",
        "/streams/fmv_08.bik",
        "/streams/fmv_09.bik",
};

struct {
    bool is_playing;

    struct {
        char path[PL_SYSTEM_MAX_PATH];
    } queue[4];
    unsigned int num_videos_queued; /* elements */
    unsigned int cur_video;         /* index */
} video;

void InitVideo(void) {
    memset(&video, 0, sizeof(video));

    av_register_all();
    avformat_network_init();


}

void ShutdownVideo(void) {

}

/***************************************************************/

void ClearVideoQueue(void) {
    memset(&video, 0, sizeof(video));
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

        video.num_videos_queued++;
    }
}

void PlayVideo(const char *path) {
    ClearVideoQueue();

    size_t len = strlen(path);
    if(len > sizeof(video.queue[0].path)) {
        LogWarn("unexpected length of path - %u bytes - expect issues!\n", len);
    }

    video.num_videos_queued = 1;
    video.cur_video         = 0;
    strncpy(video.queue[0].path, path, sizeof(video.queue[0].path));
}

void ProcessVideo(void) {
    if(!video.is_playing) {
        return;
    }
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

    mesh->texture = NULL; /* todo pass correct texture */

    plDrawMesh(mesh);
}