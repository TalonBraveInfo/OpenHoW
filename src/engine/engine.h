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

#pragma once

#include <PL/platform_graphics.h>
#include <PL/platform_math.h>
#include <PL/platform_console.h>
#include <PL/platform_model.h>
#include <PL/platform_package.h>
#include <PL/platform_filesystem.h>
#include <PL/platform_graphics_camera.h>

// remove this once everything is C++'fied
#ifdef __cplusplus
#include <string>
#include <sstream>
#include <list>
#endif

#include "../shared/util.h"

#include "pork_math.h"
#include "console.h"

#define ENGINE_TITLE        "OpenHoW"
#define ENGINE_APP_NAME     "OpenHoW"
#define ENGINE_LOG          "debug"

#define ENGINE_MAJOR_VERSION    0
#define ENGINE_MINOR_VERSION    5
#define ENGINE_PATCH_VERSION    0

#define TICKS_PER_SECOND    25
#define SKIP_TICKS          (1000 / TICKS_PER_SECOND)
#define MAX_FRAMESKIP       5

#ifdef __cplusplus
#include "resource_manager.h"

#include "audio/audio.h"
#include "game/game.h"
#include "physics/physics_interface.h"

namespace openhow {
class Engine;
extern Engine* engine;
class Engine {
 public:
  Engine();
  ~Engine();

  static AudioManager* Audio() {
    return engine->audio_manager_;
  }
  static GameManager* Game() {
    return engine->game_manager_;
  }
  static ResourceManager* Resource() {
    return engine->resource_manager_;
  }
  static IPhysicsInterface* Physics() {
    return engine->physics_interface_;
  }

  void Initialize();

  std::string GetVersionString();

  bool IsRunning();

 private:
  GameManager* game_manager_{nullptr};
  AudioManager* audio_manager_{nullptr};
  ResourceManager* resource_manager_{nullptr};
  IPhysicsInterface* physics_interface_{nullptr};
};
}

#else

typedef struct BaseGameMode BaseGameMode;

#endif // __cplusplus; todo: remove this once all code is compiled as C++

typedef struct EngineState {
  struct PLCamera* ui_camera;    // camera used for UI elements, orthographic

  unsigned int sys_ticks;
  unsigned int last_sys_tick;

  unsigned int sim_ticks;

  unsigned int draw_ticks;
  unsigned int last_draw_ms;

  struct {
    unsigned int num_chunks_drawn;
    unsigned int num_actors_drawn;
    unsigned int num_triangles_total;
  } gfx;
} EngineState;
extern EngineState g_state;

PL_EXTERN_C

/************************************************************/
/* System */

unsigned int System_GetTicks(void);

enum PromptLevel {
  PROMPT_LEVEL_DEFAULT,
  PROMPT_LEVEL_WARNING,
  PROMPT_LEVEL_ERROR,
};

void System_DisplayMessageBox(unsigned int level, const char* msg, ...);
void System_DisplayWindow(bool fullscreen, int width, int height);

int System_SetSwapInterval(int interval);
void System_SwapDisplay(void);

void System_SetWindowTitle(const char* title);
void System_GetWindowSize(int* width, int* height, bool* fs);
void System_GetWindowDrawableSize(int* width, int* height, bool* fs);
bool System_SetWindowSize(int width, int height, bool fs);

void System_PollEvents();

void System_Shutdown(void);

PL_EXTERN_C_END
