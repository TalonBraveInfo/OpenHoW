/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#ifdef WIN32
#include <winsock2.h>
#undef far
#undef near
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <assert.h>

#include "../engine.h"
#include "../frontend.h"
#include "../audio.h"
#include "../Map.h"

#include "ActorManager.h"
#include "GameManager.h"
#include "SPGameMode.h"

GameManager *GameManager::instance_ = nullptr;

GameManager::GameManager(): listener_fd(-1), server_fd(-1), server_buf_len(0) {
    char *server_ip = getenv("SERVER_IP");
    if(server_ip != NULL)
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(server_fd != -1);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(server_ip);
        addr.sin_port = htons(1236);

        assert(connect(server_fd, (struct sockaddr*)(&addr), sizeof(addr)) == 0);

        #ifdef WIN32
        u_long mode = 1;
        ioctlsocket(server_fd, FIONBIO, &mode);
        #else
        assert(fcntl(server_fd, F_SETFL, O_NONBLOCK) == 0);
        #endif
    }
    else{
        listener_fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(listener_fd != -1);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(1236);

        assert(bind(listener_fd, (struct sockaddr*)(&addr), sizeof(addr)) == 0);
        assert(listen(listener_fd, 8) == 0);

        #ifdef WIN32
        u_long mode = 1;
        ioctlsocket(listener_fd, FIONBIO, &mode);
        #else
        assert(fcntl(listener_fd, F_SETFL, O_NONBLOCK) == 0);
        #endif
    }
}

GameManager::~GameManager() {
    if(server_fd != -1)
    {
        close(server_fd);
    }
    
    while(!client_fds.empty())
    {
        close(*(client_fds.begin()));
        client_fds.erase(client_fds.begin());
    }

    if(listener_fd != -1)
    {
        close(listener_fd);
    }
}

void GameManager::Run() {
    FrontEnd_Tick();

    if(active_mode_ == nullptr) {
        return;
    }

    if(listener_fd != -1)
    {
        for(int newfd; (newfd = accept(listener_fd, NULL, 0)) != -1;)
        {
            #ifdef WIN32
            u_long mode = 1;
            ioctlsocket(newfd, FIONBIO, &mode);
            #else
            assert(fcntl(newfd, F_SETFL, O_NONBLOCK) == 0);
            #endif

            client_fds.insert(newfd);
        }
    }

    static unsigned int next_tick = 0;
    if(next_tick == 0)
    {
        next_tick = System_GetTicks();
    }
    
    auto tick = [&]()
    {
        if(ambient_emit_delay_ < g_state.sim_ticks) {
            const AudioSample* sample = ambient_samples_[rand() % MAX_AMBIENT_SAMPLES];
            if(sample != nullptr) {
                PLVector3 position = {
                        plGenerateRandomf(MAP_PIXEL_WIDTH),
                        active_map_->GetMaxHeight(),
                        plGenerateRandomf(MAP_PIXEL_WIDTH)
                };
                AudioManager::GetInstance()->PlayLocalSound(sample, position, { 0, 0, 0 }, true, 0.5f);
            }

            ambient_emit_delay_ = g_state.sim_ticks + TICKS_PER_SECOND + rand() % (7 * TICKS_PER_SECOND);
        }

        active_mode_->Tick();
    };

    while(server_fd == -1 && System_GetTicks() > next_tick)
    {
        ++g_state.sim_ticks;
        tick();
        next_tick += SKIP_TICKS;
        
        NetMessage msg;
        msg.type = NetMessage::SET_TICK;
        msg.tick_count = g_state.sim_ticks;

        for(auto c = client_fds.begin(); c != client_fds.end(); ++c)
        {
            assert(send(*c, &msg, sizeof(msg), 0) == sizeof(msg));
        }

        for(size_t i = 0; i < ActorManager::GetInstance()->actors_.size(); ++i)
        {
            const Actor *actor = ActorManager::GetInstance()->actors_[i];

            for(auto p = actor->properties_.begin(); p != actor->properties_.end(); ++p)
            {
                if(((p->second->flags & ActorProperty::INPUT) && p->second->is_dirty())
                    || p->second->dirty_ticks() > 250)
                {
                    NetMessage msg;
                    msg.type = NetMessage::SET_PROPERTY;
                    msg.actor_idx = i;
                    strcpy(msg.property_name, p->second->name.c_str());
                    p->second->to_msg(&msg);

                    for(auto c = client_fds.begin(); c != client_fds.end(); ++c)
                    {
                        assert(send(*c, &msg, sizeof(msg), 0) == sizeof(msg));
                    }

                    p->second->commit();
                }
            }
        }
    }

    while(server_fd != -1)
    {
        int r = recv(server_fd, ((char*)(&server_buf)) + server_buf_len, sizeof(server_buf) - server_buf_len, 0);
        if(r <= 0)
        {
            break;
        }

        server_buf_len += r;
        if(server_buf_len == sizeof(server_buf))
        {
            if(server_buf.type == NetMessage::SET_PROPERTY)
            {
                Actor *actor = ActorManager::GetInstance()->actors_[server_buf.actor_idx];
                ActorProperty *property = actor->properties_[server_buf.property_name];
                property->from_msg(&server_buf);
            }
            else if(server_buf.type == NetMessage::SET_TICK)
            {
                while(g_state.sim_ticks < server_buf.tick_count)
                {
                    ++g_state.sim_ticks;
                    tick();
                }
            }

            server_buf_len = 0;
        }
    }
}

void GameManager::LoadMap(const std::string &name) {
    FrontEnd_SetState(FE_MODE_LOADING);

    if(active_map_ != nullptr) {

    }

    try {
        active_map_ = new Map(name);
    } catch(const std::runtime_error &e) {
        Error("Failed to load map, aborting!\n%s\n", e.what());
    }

    MapManifest* manifest = MapManager::GetInstance()->GetManifest(name);
    std::string sample_ext = "d";
    if(manifest->time != "day") {
        sample_ext = "n";
    }

    for(unsigned int i = 1, idx = 0; i < 4; ++i) {
        if(i < 3) {
            ambient_samples_[idx++] = AudioManager::GetInstance()->CacheSample(
                    "audio/amb_" + std::to_string(i) + sample_ext + ".wav", false);
        }
        ambient_samples_[idx++] = AudioManager::GetInstance()->CacheSample("audio/batt_s" + std::to_string(i) + ".wav", false);
        ambient_samples_[idx++] = AudioManager::GetInstance()->CacheSample("audio/batt_l" + std::to_string(i) + ".wav", false);
    }

    ambient_emit_delay_ = g_state.sim_ticks + rand() % 100;

    active_mode_ = new SPGameMode();
    // call StartRound; deals with spawning everything in and other mode specific logic
    active_mode_->StartRound();

    /* todo: we should actually pause here and wait for user input
     *       otherwise players won't have time to read the loading screen */
    FrontEnd_SetState(FE_MODE_GAME);
}

void GameManager::UnloadMap() {
    for(auto & ambient_sample : ambient_samples_) {
        delete ambient_sample;
    }

    delete active_mode_;
    delete active_map_;
}

Player* GameManager::GetCurrentPlayer() {
    if(active_mode_ == nullptr) {
        // todo: return local player... ?
        return nullptr;
    }

    return active_mode_->GetCurrentPlayer();
}
