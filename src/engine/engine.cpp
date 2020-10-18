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

#include "App.h"
#include "engine.h"
#include "Language.h"
#include "config.h"
#include "input.h"
#include "Menu.h"
#include "Map.h"

#include "graphics/display.h"

EngineState g_state;

ohw::Engine *ohw::engine;

ohw::Engine::Engine() {
	g_state.gfx.numModelsDrawn = 0;
	g_state.gfx.num_chunks_drawn = 0;
	g_state.gfx.num_triangles_total = 0;
}

ohw::Engine::~Engine() {
	Display_Shutdown();

	Config_Save( Config_GetUserConfigPath() );

	delete gameManager;
	delete audioManager;
	delete resourceManager;
	delete physicsInterface;

	LanguageManager::DestroyInstance();
}

void ohw::Engine::Initialize() {


	// now initialize all other sub-systems

	Input_Initialize();
	Display_Initialize();

	resourceManager = new ResourceManager();
	audioManager = new AudioManager();
	audioManager->SetupMusicSource();
	gameManager = new GameManager();

	physicsInterface = new PhysicsInterface();

	FE_Initialize();

	Game()->CachePersistentData();
}
