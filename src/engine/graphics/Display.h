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

#include <PL/pl_graphics_camera.h>

#define DISPLAY_MIN_WIDTH    640
#define DISPLAY_MIN_HEIGHT   480

namespace ohw {
	class Display {
	public:
		Display( const char *title, int w, int h, unsigned int desiredScreen, bool fullscreen);
		~Display();

		void SetIcon( const char *path );

		void Render( double delta );

		void SetDisplaySize( int w, int h, bool fullscreen );
		void GetDisplaySize( int *w, int *h );

		void Swap();
		int SetSwapInterval( int desiredInterval );

		bool HandleEvent( const SDL_Event &event );

		void SetMousePosition( int x, int y );

		void DebugDrawLine( const PLVector3 &startPos, const PLVector3 &endPos, const PLColour &colour );

	protected:
	private:
		void RenderScene();
		void RenderSceneDebug();
		void RenderOverlays();
		void RenderDebugOverlays();

		struct DebugLine {
			PLVector3 start, end;
			PLColour colour;
		};
		std::vector< DebugLine > debugLines;

		unsigned int numDrawTicks{ 0 };
		unsigned int lastDrawTick{ 0 };
		unsigned int lastDrawMS{ 0 };

		int myDesiredScreen{ 0 };

		SDL_Window *myWindow{ nullptr };
		SDL_GLContext myGLContext{ nullptr };
	};
}

// debugging
void Display_GetFramesCount( unsigned int *fps, unsigned int *ms );
extern const char *supportedTextureFormats[];
