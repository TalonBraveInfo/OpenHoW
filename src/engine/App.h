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

#include <PL/platform.h>
#include <PL/platform_string.h>
#include <PL/platform_image.h>
#include <PL/platform_package.h>
#include <PL/platform_model.h>
#include <PL/pl_graphics.h>
#include <PL/platform_console.h>

#include "Utilities.h"
#include "Console.h"

#define APP_NAME    "OpenHoW"

#define APP_MAJOR_VERSION    0
#define APP_MINOR_VERSION    5
#define APP_PATCH_VERSION    0

#define TICKS_PER_SECOND    25
#define SKIP_TICKS          (1000 / TICKS_PER_SECOND)
#define MAX_FRAMESKIP       5

// I'm sorry, but I refuse to include the SDL2 headers here!
typedef struct SDL_Window SDL_Window;

namespace ohw {
	class ModManager;
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

		inline double GetDeltaTime() const {
			return deltaTime;
		}

		void InitializeConfig();
		void InitializeDisplay();

		void SwapDisplay();
		int SetSwapInterval( int interval );

		void SetDisplaySize( int width, int height, bool fullscreen );
		void GetDisplaySize( int *width, int *height );

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

	private:
		void CreateDisplay( int w, int h, int flags );

		///////////////////////////////////////////////
		// Events
	public:
		void PollEvents();

		///////////////////////////////////////////////
		// System

	public:
		static const char *GetVersionString();

		bool IsRunning();

		static void *MAlloc( size_t size, bool abortOnFail );
		static void *CAlloc( size_t num, size_t size, bool abortOnFail );

	private:
		static const char *GetClipboardText( void * );
		static void SetClipboardText( void *, const char *text );

		void SetWindowIcon( const char *path );

		SDL_Window *myWindow{ nullptr };
		void *myGLContext{ nullptr };

		std::vector< DisplayPreset > myDisplayPresets;
		int myDesiredDisplay{ 0 };

		ModManager *myModManager;

		double deltaTime;
		unsigned int numSysTicks{ 0 };
		unsigned int lastSysTick{ 0 };
		unsigned int numDrawTicks{ 0 };
		unsigned int lastDrawTick{ 0 };
		unsigned int lastDrawMS{ 0 };
		unsigned int numSimTicks{ 0 };
	};

	App *GetApp();
}
