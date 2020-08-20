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

#include "engine.h"
#include "language.h"
#include "mod_support.h"
#include "client.h"
#include "config.h"
#include "input.h"
#include "frontend.h"
#include "Map.h"
#include "imgui_layer.h"

#include "graphics/display.h"

EngineState g_state;

ohw::Engine *ohw::engine;

ohw::Engine::Engine() {
	g_state.draw_ticks = 0;

	g_state.last_draw_ms = 0;
	g_state.last_sys_tick = 0;
	g_state.sim_ticks = 0;
	g_state.sys_ticks = 0;

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
	LogInfo( "Initializing Engine (%s)...\n", GetVersionString().c_str() );

	Console_Initialize();

	// load in the manifests
	Mod_RegisterMods();

	// check for any command line arguments
	const char *var = plGetCommandLineArgumentValue( "-mod" );
	if ( var == nullptr ) {
		// otherwise default to base campaign
		var = "how";
	}

	Mod_SetMod( var );

	// Initialize the language manager
	LanguageManager::GetInstance()->SetLanguage( "eng" );

	/* this MUST be done after all vars have been
	 * initialized, otherwise, right now, certain
	 * vars will not be loaded/saved! */
	Config_Load( CONFIG_FILENAME );

	// now initialize all other sub-systems

	Input_Initialize();
	Display_Initialize();

	resourceManager = new ResourceManager();
	audioManager = new AudioManager();
	audioManager->SetupMusicSource();
	gameManager = new GameManager();

	physicsInterface = new PhysicsInterface();

	FE_Initialize();

	plParseConsoleString( "fsListMounted" );

	Game()->CachePersistentData();
}

std::string ohw::Engine::GetVersionString() {
	return "v" +
		std::to_string( ENGINE_MAJOR_VERSION ) + "." +
		std::to_string( ENGINE_MINOR_VERSION ) + "." +
		std::to_string( ENGINE_PATCH_VERSION ) + "-" +
		GIT_BRANCH + ":" + GIT_COMMIT_HASH + "-" + GIT_COMMIT_COUNT;
}

bool ohw::Engine::IsRunning() {
	System_PollEvents();

	static unsigned int next_tick = 0;
	if ( next_tick == 0 ) {
		next_tick = System_GetTicks();
	}

	unsigned int loops = 0;
	while ( System_GetTicks() > next_tick && loops < MAX_FRAMESKIP ) {
		g_state.sys_ticks = System_GetTicks();
		g_state.sim_ticks++;

		Client_ProcessInput(); // todo: kill this

		Physics()->Tick();
		Game()->Tick();
		Audio()->Tick();

		g_state.last_sys_tick = System_GetTicks();
		next_tick += SKIP_TICKS;
		loops++;
	}

	deltaTime = ( double ) ( System_GetTicks() + SKIP_TICKS - next_tick ) / ( double ) ( SKIP_TICKS );
	Display_Draw( deltaTime );

	return true;
}
