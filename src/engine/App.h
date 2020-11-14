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

#include <vector>
#include <list>

#include <PL/platform.h>
#include <PL/platform_string.h>
#include <PL/platform_image.h>
#include <PL/platform_package.h>
#include <PL/platform_model.h>
#include <PL/pl_graphics.h>
#include <PL/platform_console.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "Utilities.h"
#include "Timer.h"
#include "Console.h"

#include "graphics/Display.h"

#define APP_NAME    "OpenHoW"

#define APP_MAJOR_VERSION    0
#define APP_MINOR_VERSION    5
#define APP_PATCH_VERSION    0

#define TICKS_PER_SECOND    25
#define SKIP_TICKS          (1000 / TICKS_PER_SECOND)
#define MAX_FRAMESKIP       5

#include "ModManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "GameManager.h"
#include "AudioManager.h"

namespace ohw {
	class App {
	public:
		App( int argc, char **argv );

		void Shutdown();

		enum class MBErrorLevel {
			INFORMATION_MSG,
			WARNING_MSG,
			ERROR_MSG,
		};
		void DisplayMessageBox( MBErrorLevel level, const char *message, ... );

		inline unsigned int GetTicks() const {
			return numSysTicks;
		}
		inline unsigned int GetSimulationTicks() const {
			return numSimTicks;
		}
		inline double GetDeltaTime() const {
			return deltaTime;
		}

		void InitializeConfig();
		void InitializeDisplay();
		void InitializeAudio();
		void InitializeGame();

		inline Display *GetDisplay() { return myDisplay; }

		struct DisplayPreset {
			DisplayPreset( int w, int h, int r ) : width( w ), height( h ), refresh( r ) {}

			int width;
			int height;
			int refresh;
		};
		typedef std::vector< DisplayPreset > DisplayPresetVector;
		inline const DisplayPresetVector *GetDisplayPresets() const {
			return &myDisplayPresets;
		}

		static void SetClipboardText( void *, const char *text );
		static const char *GetClipboardText( void * );

		///////////////////////////////////////////////
		// Events
	public:
		void PollEvents();

		///////////////////////////////////////////////
		// System

		static const char *GetVersionString();

		bool IsRunning();

		static void *MAlloc( size_t size, bool abortOnFail );
		static void *CAlloc( size_t num, size_t size, bool abortOnFail );

		InputManager *inputManager;
		ModManager *modManager;
		GameManager *gameManager;
		AudioManager *audioManager;
		ResourceManager *resourceManager;

		//////////////////////////////////////////////////////
		// PROFILING

		void StartProfilingGroup( const char *identifier );
		void EndProfilingGroup( const char *identifier );

		typedef std::map< std::string, Timer > ProfilerMap;
		const ProfilerMap &GetProfilers() const { return myProfilers; }

	private:
		Display *myDisplay;

		std::vector< DisplayPreset > myDisplayPresets;

		double deltaTime;
		unsigned int numSysTicks{ 0 };
		unsigned int lastSysTick{ 0 };
		unsigned int numSimTicks{ 0 };

		ProfilerMap myProfilers;
	};

	App *GetApp();
}

#if defined( DEBUG_BUILD )
#   define START_MEASURE() ohw::GetApp()->StartProfilingGroup( __PRETTY_FUNCTION__ )
#   define END_MEASURE()   ohw::GetApp()->EndProfilingGroup( __PRETTY_FUNCTION__ )
#else
#   define START_MEASURE()
#   define END_MEASURE()
#endif
