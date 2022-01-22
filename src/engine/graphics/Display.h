// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

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
