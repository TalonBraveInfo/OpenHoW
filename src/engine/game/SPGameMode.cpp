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
#define close(x) closesocket(x)
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <assert.h>
#include <PL/platform_graphics_camera.h>

#include "../engine.h"
#include "../frontend.h"
#include "../audio.h"

#include "SPGameMode.h"
#include "ActorManager.h"

SPGameMode::SPGameMode(): listener_fd(-1), server_fd(-1), server_buf_len(0) {
    char *server_ip = getenv("SERVER_IP");
    if(server_ip != NULL)
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(server_fd != -1);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(server_ip);
        addr.sin_port = htons(1235);

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
        addr.sin_port = htons(1235);

        assert(bind(listener_fd, (struct sockaddr*)(&addr), sizeof(addr)) == 0);
        assert(listen(listener_fd, 8) == 0);

        #ifdef WIN32
        u_long mode = 1;
        ioctlsocket(listener_fd, FIONBIO, &mode);
        #else
        assert(fcntl(listener_fd, F_SETFL, O_NONBLOCK) == 0);
        #endif
    }

    players_.resize(4);
}

SPGameMode::~SPGameMode() {
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

    AudioManager::GetInstance()->FreeSources();
    AudioManager::GetInstance()->FreeSamples();

    DestroyActors();
}

void SPGameMode::StartRound() {
    if(HasRoundStarted()) {
        Error("Attempted to change map in the middle of a round, aborting!\n");
    }

    SpawnActors();

    round_started_ = true;
}

void SPGameMode::RestartRound() {
    DestroyActors();
    SpawnActors();
}

void SPGameMode::EndRound() {
    DestroyActors();
}

void SPGameMode::Tick() {
    if(!HasRoundStarted()) {
        // still setting the game up...
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

    Actor* slave = GetCurrentPlayer()->input_target;
    if(slave != nullptr) {
        if(listener_fd != -1)
        {
            slave->HandleInput();
        }

        // temp: force the camera at the actor pos
        g_state.camera->position = slave->GetPosition();
        g_state.camera->angles = slave->GetAngles();
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

    while(server_fd != -1)
    {
        int r = recv(server_fd, ((unsigned char*)(&server_buf)) + server_buf_len, sizeof(server_buf) - server_buf_len, 0);
        if(r <= 0)
        {
            break;
        }

        server_buf_len += r;
        if(server_buf_len == sizeof(server_buf))
        {
            Actor *actor = ActorManager::GetInstance()->actors_[server_buf.actor_idx];
            ActorProperty *property = actor->properties_[server_buf.property_name];
            property->from_msg(&server_buf);

            server_buf_len = 0;
        }
    }

    ActorManager::GetInstance()->TickActors();
}

void SPGameMode::SpawnActors() {
    Map* map = GameManager::GetInstance()->GetCurrentMap();
    u_assert(map != nullptr);

    std::vector<MapSpawn> spawns = map->GetSpawns();
    for(auto spawn : spawns) {
        Actor* actor = ActorManager::GetInstance()->SpawnMapActor(spawn.name);
        if(actor == nullptr) {
            continue;
        }

        actor->SetPosition(PLVector3(spawn.position[0], spawn.position[1], spawn.position[2]));
        actor->SetAngles(PLVector3(spawn.angles[0] / 360, spawn.angles[1] / 360, spawn.angles[2] / 360));

        // todo: assign player pigs etc., temp hack
        if(strcmp(spawn.name, "GR_ME") == 0) {
            players_[0].input_target = actor;
        }
    }
}

void SPGameMode::DestroyActors() {
    ActorManager::GetInstance()->DestroyActors();
}

void SPGameMode::StartTurn() {
    Player *player = GetCurrentPlayer();
    if(player->input_target == nullptr) {
        LogWarn("No valid control target for player \"%s\"!\n", player->name.c_str());
        EndTurn();
        return;
    }
}

void SPGameMode::EndTurn() {
    // move onto the next player
    if(++current_player_ >= players_.size()) {
        current_player_ = 0;
    }
}

void SPGameMode::PlayerJoined(Player *player) {

}

void SPGameMode::PlayerLeft(Player *player) {

}

unsigned int SPGameMode::GetMaxSpectators() const {
    return 0;
}

void SPGameMode::SpectatorJoined(Player *player) {

}

void SPGameMode::SpectatorLeft(Player *player) {

}

unsigned int SPGameMode::GetMaxPlayers() const {
    return 0;
}
