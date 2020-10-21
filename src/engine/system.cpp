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
#include "InputManager.h"
#include "imgui_layer.h"

#include "../3rdparty/imgui/examples/imgui_impl_sdl.h"
#include "../3rdparty/imgui/examples/imgui_impl_opengl3.h"

void System_DisplayWindow( bool fullscreen, int width, int height ) {
	/* setup imgui integration */

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	//io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	io.KeyMap[ ImGuiKey_Tab ] = SDL_SCANCODE_TAB;
	io.KeyMap[ ImGuiKey_LeftArrow ] = SDL_SCANCODE_LEFT;
	io.KeyMap[ ImGuiKey_RightArrow ] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ ImGuiKey_UpArrow ] = SDL_SCANCODE_UP;
	io.KeyMap[ ImGuiKey_DownArrow ] = SDL_SCANCODE_DOWN;
	io.KeyMap[ ImGuiKey_PageUp ] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ ImGuiKey_PageDown ] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ ImGuiKey_Home ] = SDL_SCANCODE_HOME;
	io.KeyMap[ ImGuiKey_End ] = SDL_SCANCODE_END;
	io.KeyMap[ ImGuiKey_Insert ] = SDL_SCANCODE_INSERT;
	io.KeyMap[ ImGuiKey_Delete ] = SDL_SCANCODE_DELETE;
	io.KeyMap[ ImGuiKey_Backspace ] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ ImGuiKey_Space ] = SDL_SCANCODE_SPACE;
	io.KeyMap[ ImGuiKey_Enter ] = SDL_SCANCODE_RETURN;
	io.KeyMap[ ImGuiKey_Escape ] = SDL_SCANCODE_ESCAPE;
	io.KeyMap[ ImGuiKey_A ] = SDL_SCANCODE_A;
	io.KeyMap[ ImGuiKey_C ] = SDL_SCANCODE_C;
	io.KeyMap[ ImGuiKey_V ] = SDL_SCANCODE_V;
	io.KeyMap[ ImGuiKey_X ] = SDL_SCANCODE_X;
	io.KeyMap[ ImGuiKey_Y ] = SDL_SCANCODE_Y;
	io.KeyMap[ ImGuiKey_Z ] = SDL_SCANCODE_Z;

	io.SetClipboardTextFn = System_SetClipboardText;
	io.GetClipboardTextFn = System_GetClipboardText;
	io.ClipboardUserData = nullptr;

#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	io.ImeWindowHandle = wmInfo.info.win.window;
#endif

	ImGui_ImplOpenGL3_Init();

#if 0
	// https://github.com/ocornut/imgui/issues/707#issuecomment-468798935
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		/// 0 = FLAT APPEARENCE
		/// 1 = MORE "3D" LOOK
		float is3D = 1;

		colors[ ImGuiCol_Text ] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
		colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.40f, 0.40f, 0.40f, 1.00f );
		colors[ ImGuiCol_ChildBg ] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
		colors[ ImGuiCol_WindowBg ] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
		colors[ ImGuiCol_PopupBg ] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
		colors[ ImGuiCol_Border ] = ImVec4( 0.12f, 0.12f, 0.12f, 0.71f );
		colors[ ImGuiCol_BorderShadow ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.06f );
		colors[ ImGuiCol_FrameBg ] = ImVec4( 0.42f, 0.42f, 0.42f, 0.54f );
		colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.42f, 0.42f, 0.42f, 0.40f );
		colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.56f, 0.56f, 0.56f, 0.67f );
		colors[ ImGuiCol_TitleBg ] = ImVec4( 0.19f, 0.19f, 0.19f, 1.00f );
		colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.22f, 0.22f, 0.22f, 1.00f );
		colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.17f, 0.17f, 0.17f, 0.90f );
		colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.335f, 0.335f, 0.335f, 1.000f );
		colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.24f, 0.24f, 0.24f, 0.53f );
		colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
		colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.52f, 0.52f, 0.52f, 1.00f );
		colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.76f, 0.76f, 0.76f, 1.00f );
		colors[ ImGuiCol_CheckMark ] = ImVec4( 0.65f, 0.65f, 0.65f, 1.00f );
		colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.52f, 0.52f, 0.52f, 1.00f );
		colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.64f, 0.64f, 0.64f, 1.00f );
		colors[ ImGuiCol_Button ] = ImVec4( 0.54f, 0.54f, 0.54f, 0.35f );
		colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.52f, 0.52f, 0.52f, 0.59f );
		colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.76f, 0.76f, 0.76f, 1.00f );
		colors[ ImGuiCol_Header ] = ImVec4( 0.38f, 0.38f, 0.38f, 1.00f );
		colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.47f, 0.47f, 0.47f, 1.00f );
		colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.76f, 0.76f, 0.76f, 0.77f );
		colors[ ImGuiCol_Separator ] = ImVec4( 0.000f, 0.000f, 0.000f, 0.137f );
		colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.700f, 0.671f, 0.600f, 0.290f );
		colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.702f, 0.671f, 0.600f, 0.674f );
		colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0.26f, 0.59f, 0.98f, 0.25f );
		colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.26f, 0.59f, 0.98f, 0.67f );
		colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.26f, 0.59f, 0.98f, 0.95f );
		colors[ ImGuiCol_PlotLines ] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
		colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
		colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
		colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
		colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.73f, 0.73f, 0.73f, 0.35f );
		colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );
		colors[ ImGuiCol_DragDropTarget ] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
		colors[ ImGuiCol_NavHighlight ] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
		colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
		colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );

		style.PopupRounding = 3;

		style.WindowPadding = ImVec2( 4, 4 );
		style.FramePadding = ImVec2( 6, 4 );
		style.ItemSpacing = ImVec2( 6, 2 );

		style.ScrollbarSize = 18;

		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = is3D;

		style.WindowRounding = 3;
		style.ChildRounding = 3;
		style.FrameRounding = 3;
		style.ScrollbarRounding = 2;
		style.GrabRounding = 3;

#ifdef IMGUI_HAS_DOCK
		style.TabBorderSize = is3D;
		style.TabRounding   = 3;

		colors[ImGuiCol_DockingEmptyBg]     = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		colors[ImGuiCol_Tab]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TabHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_TabActive]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		colors[ImGuiCol_TabUnfocused]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		colors[ImGuiCol_DockingPreview]     = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
#endif
	}
#endif

	io.Fonts->Clear();

	// Load in the default font

	const char* fontPath = "fonts/OpenSans-SemiBold.ttf";
	PLFile* fontFile = plOpenFile( fontPath, false );
	if ( fontFile == nullptr ) {
		Warning( "Failed to load font, \"%s\", for ImGui (%s)! Falling back to default...\n", fontPath, plGetError() );
		io.Fonts->AddFontDefault();
		return;
	}

	size_t fileSize = plGetFileSize( fontFile );
	uint8_t* buf = new uint8_t[fileSize];
	plReadFile( fontFile, buf, 1, fileSize );
	plCloseFile( fontFile );

	if ( io.Fonts->AddFontFromMemoryTTF( buf, fileSize, 16.f, nullptr, nullptr ) == nullptr ) {
		Warning( "Failed to add font from memory! Falling back to default...\n" );
		io.Fonts->AddFontDefault();
	}
}

///////////////////////////////////////////////////
