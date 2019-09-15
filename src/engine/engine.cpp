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

#include <PL/platform_filesystem.h>

#include "engine.h"
#include "language.h"
#include "model.h"
#include "mod_support.h"
#include "client.h"
#include "config.h"
#include "input.h"
#include "frontend.h"
#include "Map.h"

#include "graphics/display.h"

EngineState g_state;

#include "imgui_layer.h"

#include "game/actor_manager.h"

// Temporary interface, since graphics sub-system is written in C :^)
extern "C" void DrawActors(void) {
  ActorManager::GetInstance()->DrawActors();
}

// Temporary interface, since graphics sub-system is written in C :^)
extern "C" void DrawMap(void) {
  if (!cv_graphics_draw_world->b_value) {
    return;
  }

  Map* map = openhow::engine->GetGameManager()->GetCurrentMap();
  if (map == nullptr) {
    return;
  }

  map->Draw();
}

openhow::Engine* openhow::engine;

openhow::Engine::Engine() {
  g_state.draw_ticks = 0;

  g_state.last_draw_ms = 0;
  g_state.last_sys_tick = 0;
  g_state.sim_ticks = 0;
  g_state.sys_ticks = 0;

  g_state.gfx.num_actors_drawn = 0;
  g_state.gfx.num_chunks_drawn = 0;
  g_state.gfx.num_triangles_total = 0;

  u_init_paths();
}

openhow::Engine::~Engine() {
  Display_Shutdown();

  Config_Save(Config_GetUserConfigPath());

  delete game_manager_;
  delete audio_manager_;
}

void openhow::Engine::Initialize() {
  LogInfo("Initializing Engine (%s)...\n", GetVersionString().c_str());

  Console_Initialize();

  // check for any command line arguments

  const char* var;
  if ((var = plGetCommandLineArgumentValue("-path")) != nullptr) {
    if (!plPathExists(var)) {
      LogWarn("invalid path \"%s\", does not exist, ignoring!\n");
    }

    u_set_base_path(var);
  }

  /* this MUST be done after all vars have been
   * initialized, otherwise, right now, certain
   * vars will not be loaded/saved! */
  Config_Load(Config_GetUserConfigPath());

  plRegisterStandardPackageLoaders();

  // load in the manifests

  Languages_Initialize();
  Mod_RegisterCampaigns();

  if ((var = plGetCommandLineArgumentValue("-mod")) == nullptr &&
      (var = plGetCommandLineArgumentValue("-campaign")) == nullptr) {
    // otherwise default to base campaign
    var = "how";
  }

  Mod_SetCampaign(var);

  // now initialize all other sub-systems

  Input_Initialize();
  Display_Initialize();
  audio_manager_ = new AudioManager();
  game_manager_ = new GameManager();
  FE_Initialize();
  ModelManager::GetInstance();

  // Ensure that our manifest list is updated
  GetGameManager()->RegisterMapManifests();

  LogInfo("Base path:         \"%s\"\n", u_get_base_path());
  LogInfo("Campaign path:     \"%s\"\n", u_get_full_path());
  LogInfo("Working directory: \"%s\"\n", plGetWorkingDirectory());
}

std::string openhow::Engine::GetVersionString() {
  return "v" +
      std::to_string(ENGINE_MAJOR_VERSION) + "." +
      std::to_string(ENGINE_MINOR_VERSION) + "." +
      std::to_string(ENGINE_PATCH_VERSION) + "-" +
      GIT_BRANCH + ":" + GIT_COMMIT_HASH + "-" + GIT_COMMIT_COUNT;
}

bool openhow::Engine::IsRunning() {
  System_PollEvents();

  static unsigned int next_tick = 0;
  if (next_tick == 0) {
    next_tick = System_GetTicks();
  }

  unsigned int loops = 0;
  while (System_GetTicks() > next_tick && loops < MAX_FRAMESKIP) {
    g_state.sys_ticks = System_GetTicks();
    g_state.sim_ticks++;

    Client_ProcessInput(); // todo: kill this

    GetGameManager()->Tick();
    GetAudioManager()->Tick();

    g_state.last_sys_tick = System_GetTicks();
    next_tick += SKIP_TICKS;
    loops++;
  }

  ImGuiImpl_SetupFrame();

  double delta_time = (double) (System_GetTicks() + SKIP_TICKS - next_tick) / (double) (SKIP_TICKS);
  Display_SetupDraw(delta_time);

  Display_DrawScene();
  Display_DrawInterface();
  Display_DrawDebug();

  ImGuiImpl_Draw();

  Display_Flush();

  return true;
}
