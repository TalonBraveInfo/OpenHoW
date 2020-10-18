/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

// remove this once everything is C++'fied
#ifdef __cplusplus
#include <string>
#include <sstream>
#include <list>
#include <array>
#endif

#include "pork_math.h"
#include "Console.h"
#include "ResourceManager.h"
#include "audio.h"
#include "game.h"
#include "PhysicsInterface.h"

namespace ohw {

class Engine;
extern Engine *engine;

class Engine {
public:
	Engine();
	~Engine();

	static AudioManager *Audio() {
		return engine->audioManager;
	}
	static GameManager *Game() {
		return engine->gameManager;
	}
	static ResourceManager *Resource() {
		return engine->resourceManager;
	}
	static PhysicsInterface *Physics() {
		return engine->physicsInterface;
	}

	void Initialize();

	bool IsRunning();

	double GetDeltaTime() { return deltaTime; }

private:
	GameManager *gameManager{ nullptr };
	AudioManager *audioManager{ nullptr };
	ResourceManager *resourceManager{ nullptr };
	PhysicsInterface *physicsInterface{ nullptr };

	double deltaTime{ 0 };
};

}

typedef struct EngineState {
	struct PLCamera *ui_camera;    // camera used for UI elements, orthographic

	struct {
		unsigned int num_chunks_drawn;
		unsigned int numModelsDrawn;
		unsigned int num_triangles_total;
	} gfx;
} EngineState;
extern EngineState g_state;
