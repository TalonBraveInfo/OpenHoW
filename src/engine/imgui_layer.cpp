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
#include "imgui_layer.h"
#include "graphics/display.h"
#include "graphics/Camera.h"

#include <SDL2/SDL_syswm.h>

#undef far
#undef near

#include "../3rdparty/imgui/examples/imgui_impl_opengl3.h"

#include "editor/BaseWindow.h"
#include "editor/MapConfigEditor.h"
#include "editor/TextureViewer.h"
#include "editor/NewMapWindow.h"
#include "editor/ModelViewer.h"
#include "editor/TerrainImportWindow.h"
#include "editor/ActorTreeWindow.h"
#include "editor/NewGameWindow.h"
#include "editor/ParticleEditor.h"
#include "editor/TexturePicker.h"

#include "Language.h"

static bool show_quit = false;
static bool show_file = false;
static bool show_console = false;
static bool show_settings = false;

static std::vector< BaseWindow * > windows;

static PLCamera *imguiCamera = nullptr;

using namespace ohw;

#define IMGUI_POS_CENTER \
    ImVec2( \
    static_cast<float>(cv_display_width->i_value) * 0.5f, \
    static_cast<float>(cv_display_height->i_value) * 0.5f ), \
    ImGuiCond_Once, \
    ImVec2( 0.5f, 0.5f )

void ImGuiImpl_Setup() {
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();

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

	io.SetClipboardTextFn = GetApp()->SetClipboardText;
	io.GetClipboardTextFn = GetApp()->GetClipboardText;
	io.ClipboardUserData = nullptr;

#if 0
#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	io.ImeWindowHandle = wmInfo.info.win.window;
#endif
#endif

	ImGui_ImplOpenGL3_Init();


	io.Fonts->Clear();

	// Load in the default font

	const char *fontPath = "fonts/OpenSans-SemiBold.ttf";
	PLFile *fontFile = plOpenFile( fontPath, false );
	if ( fontFile == nullptr ) {
		Warning( "Failed to load font, \"%s\", for ImGui (%s)! Falling back to default...\n", fontPath, plGetError() );
		io.Fonts->AddFontDefault();
		return;
	}

	size_t fileSize = plGetFileSize( fontFile );
	uint8_t *buf = new uint8_t[fileSize];
	plReadFile( fontFile, buf, 1, fileSize );
	plCloseFile( fontFile );

	if ( io.Fonts->AddFontFromMemoryTTF( buf, fileSize, 16.f, nullptr, nullptr ) == nullptr ) {
		Warning( "Failed to add font from memory! Falling back to default...\n" );
		io.Fonts->AddFontDefault();
	}

	imguiCamera = plCreateCamera();
	if ( imguiCamera == nullptr ) {
		Error( "failed to create ui camera, aborting!\n%s\n", plGetError() );
	}

	imguiCamera->mode = PL_CAMERA_MODE_ORTHOGRAPHIC;
	imguiCamera->fov = 90;
	imguiCamera->near = 0;
	imguiCamera->far = 1000;
	imguiCamera->viewport.w = cv_display_width->i_value;
	imguiCamera->viewport.h = cv_display_height->i_value;
}

void ImGuiImpl_SetupCamera( void ) {

}

void ImGuiImpl_SetupFrame( void ) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void ImGuiImpl_Draw( void ) {
	ImGui::Render();

	plSetupCamera( imguiCamera );
	plSetShaderProgram( nullptr );

	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}

/************************************************************/
/* Settings */

#if 0 // TODO: REDO
void UI_DisplaySettings() {
	if ( !show_settings ) {
		return;
	}

	ImGui::Begin( "Settings", &show_settings );

	static int item_current = 0;
	bool display_changed = false;
	const ohw::App::DisplayPreset *mode = GetApp()->GetVideoPreset( item_current );
	char s[32] = { 0 };
	snprintf( s, 32, "%dx%d", mode->width, mode->height );
	if ( ImGui::BeginCombo( "Resolution", s, 0 ) ) {
		int num_modes = Display_GetNumVideoPresets();
		for ( int n = 0; n < num_modes; n++ ) {
			mode = Display_GetVideoPreset( n );
			snprintf( s, 32, "%dx%d", mode->width, mode->height );
			bool is_selected = ( item_current == n );
			if ( ImGui::Selectable( s, is_selected ) ) {
				item_current = n;
				display_changed = true;
				break;
			}
			if ( is_selected )
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	static bool fs;
	bool fs_changed = false;
	fs_changed |= ImGui::Checkbox( "Fullscreen", &fs );

	ImGui::SameLine();

	bool vsync = cv_display_vsync->b_value;
	if ( ImGui::Checkbox( "Vsync", &vsync ) && vsync != cv_display_vsync->b_value ) {
		plSetConsoleVariable( cv_display_vsync, vsync ? "true" : "false" );
	}

	static int aspect_opt = 0;
	bool aspect_changed = false;
	ImGui::Text( "Aspect ratio:" );
	aspect_changed |= ImGui::RadioButton( "4:3", &aspect_opt, 0 );
	ImGui::SameLine();
	aspect_changed |= ImGui::RadioButton( "Fit window", &aspect_opt, 1 );

	static int ui_scale_opt = 0;
	bool ui_scale_changed = false;
	ImGui::Text( "UI scale:" );
	ui_scale_changed |= ImGui::RadioButton( "Auto", &ui_scale_opt, 0 );
	ImGui::SameLine();
	ui_scale_changed |= ImGui::RadioButton( "1x", &ui_scale_opt, 1 );
	ImGui::SameLine();
	ui_scale_changed |= ImGui::RadioButton( "2x", &ui_scale_opt, 2 );
	ImGui::SameLine();
	ui_scale_changed |= ImGui::RadioButton( "3x", &ui_scale_opt, 3 );
	ImGui::SameLine();
	ui_scale_changed |= ImGui::RadioButton( "4x", &ui_scale_opt, 4 );

	static int filter_textures = -1;
	if ( filter_textures == -1 ) { filter_textures = cv_graphics_texture_filter->b_value; }
	if ( ImGui::Checkbox( "Filter textures", reinterpret_cast<bool *>(&filter_textures) ) ) {
		plSetConsoleVariable( cv_graphics_texture_filter, filter_textures ? "true" : "false" );
	}

	//Apply changes
	char buf[16];
	if ( display_changed ) {
		plSetConsoleVariable( cv_display_width, pl_itoa( mode->width, buf, 16, 10 ) );
		plSetConsoleVariable( cv_display_height, pl_itoa( mode->height, buf, 16, 10 ) );
		System_SetWindowSize( cv_display_width->i_value, cv_display_height->i_value, cv_display_fullscreen->b_value );
	}
	if ( fs_changed ) {
		plSetConsoleVariable( cv_display_fullscreen, fs ? "true" : "false" );
		System_SetWindowSize( cv_display_width->i_value, cv_display_height->i_value, cv_display_fullscreen->b_value );
	}
	if ( aspect_changed ) {
		plSetConsoleVariable( cv_display_use_window_aspect, aspect_opt ? "true" : "false" );
		Display_UpdateViewport( 0, 0, cv_display_width->i_value, cv_display_height->i_value );
	}
	if ( ui_scale_changed ) {
		plSetConsoleVariable( cv_display_ui_scale, pl_itoa( ui_scale_opt, buf, 16, 10 ) );
	}

	if ( ImGui::Button( "Cancel" ) ) {
		show_settings = false;
	}

	ImGui::End();
}

/************************************************************/

enum {
	FILE_TYPE_UNKNOWN,
	FILE_TYPE_MAP_POG,
	FILE_TYPE_MAP_PTG,
	FILE_TYPE_MAP_PMG,
	FILE_TYPE_IMAGE,
	FILE_TYPE_MODEL,
	FILE_TYPE_AUDIO,
	FILE_TYPE_PARTICLE,
};

typedef struct FileDescriptor {
	char path[PL_SYSTEM_MAX_PATH];
	unsigned int type;
} FileDescriptor;

static std::vector< FileDescriptor > file_list;

void AddFilePath( const char *path, void *userData ) {
	u_unused( userData );

	FileDescriptor descriptor;
	strncpy( descriptor.path, path, sizeof( descriptor.path ) );
	descriptor.type = FILE_TYPE_UNKNOWN;

	const char *ext = plGetFileExtension( path );
	if ( ext != nullptr ) {
		if (
				strcmp( ext, "tim" ) == 0 ||
				strcmp( ext, "bmp" ) == 0 ||
				strcmp( ext, "png" ) == 0 ) {
			descriptor.type = FILE_TYPE_IMAGE;
		} else if (
				strcmp( ext, "pps" ) == 0
				) {
			descriptor.type = FILE_TYPE_PARTICLE;
		} else if (
				strcmp( ext, "wav" ) == 0
				) {
			descriptor.type = FILE_TYPE_AUDIO;
		} else if (
				strcmp( ext, "ptg" ) == 0
				) {
			descriptor.type = FILE_TYPE_MAP_PTG;
		} else if (
				strcmp( ext, "pog" ) == 0
				) {
			descriptor.type = FILE_TYPE_MAP_POG;
		} else if (
				strcmp( ext, "pmg" ) == 0
				) {
			descriptor.type = FILE_TYPE_MAP_PMG;
		} else if (
				pl_strcasecmp( ext, "vtx" ) == 0 ||
				pl_strcasecmp( ext, "min" ) == 0 ||
				pl_strcasecmp( ext, "obj" ) == 0 ) {
			descriptor.type = FILE_TYPE_MODEL;
		}
	}

	file_list.push_back( descriptor );
}

void ScanDirectories() {
	file_list.clear();
	plScanDirectory( ".", nullptr, AddFilePath, true, nullptr );
}

void UI_DisplayFileBox() {
	if ( !show_file ) {
		return;
	}

	static bool has_scanned = false;
	if ( !has_scanned ) {
		ScanDirectories();
		has_scanned = true;
	}

	ImGui::SetNextWindowPos( IMGUI_POS_CENTER );
	ImGui::SetNextWindowSize( ImVec2( 512, 512 ), ImGuiCond_Once );
	ImGui::Begin( "Open File", &show_file );

	static ImGuiTextFilter filter;
	filter.Draw();
	ImGui::SameLine();

	if ( ImGui::Button( "Rescan", ImVec2( ImGui::GetWindowContentRegionWidth(), 0 ) ) ) {
		ScanDirectories();
	}

	ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 5.0f );
	ImGui::PushStyleVar( ImGuiStyleVar_ButtonTextAlign, ImVec2( -1.f, 0 ) );
	ImGui::BeginChild( "Child2", ImVec2( ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight() - 64 ), true );
	ImGui::Columns( 2 );
	for ( auto &i : file_list ) {
		if ( filter.PassFilter( i.path ) ) {
			if ( ImGui::Button( i.path ) ) {
				switch ( i.type ) {
					case FILE_TYPE_IMAGE: {
						try {
							auto *viewer = new TextureViewer( i.path );
							windows.push_back( viewer );
						} catch ( const std::runtime_error &error ) {
							Warning( "%s\n", error.what() );
						}
					}
						break;
#if 0
						case FILE_TYPE_MODEL:
							try {
								auto* viewer = new ModelViewer( i.path );
								windows.push_back( viewer );
							} catch ( const std::runtime_error& error ) {
								LogWarn( "%s\n", error.what() );
							}
							break;
#endif
					case FILE_TYPE_AUDIO:
						Engine::Audio()->PlayGlobalSound( i.path );
						break;

					default:
						break;
				}
			}

			ImGui::NextColumn();

			const char *type = "Unknown";
			switch ( i.type ) {
				case FILE_TYPE_AUDIO:
					type = "Audio";
					break;
				case FILE_TYPE_PARTICLE:
					type = "Particle System";
					break;
				case FILE_TYPE_IMAGE:
					type = "Image";
					break;
				case FILE_TYPE_MAP_POG:
					type = "Map Objects";
					break;
				case FILE_TYPE_MAP_PTG:
					type = "Map Textures";
					break;
				case FILE_TYPE_MAP_PMG:
					type = "Map Geometry";
					break;
				default:
					break;
			}

			ImGui::Text( "%s", type );

			ImGui::NextColumn();
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	ImGui::End();
}
#endif

class QuitWindow : public BaseWindow {
public:
	void Display() override {
		ImGui::SetNextWindowPos( IMGUI_POS_CENTER );
		ImGui::Begin( "Are you sure?", &show_quit, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
		                                           ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings );
		ImGui::Text( "Are you sure you want to quit the game?\nAny unsaved changes will be lost!\n" );
		ImGui::Dummy( ImVec2( 0, 5 ) );
		ImGui::Separator();
		ImGui::Dummy( ImVec2( 0, 5 ) );

		if ( ImGui::Button( "Yes", ImVec2( 64, 0 ) ) ) {
			ImGui::End();
			GetApp()->Shutdown();
		}

		ImGui::SameLine();

		if ( ImGui::Button( "No", ImVec2( 64, 0 ) ) ) {
			ToggleStatus();
		}

		ImGui::End();
	}

protected:
private:
};

class ConsoleWindow : public BaseWindow {
public:
	void SendCommand() {
		plParseConsoleString( input_buf_ );
		strncpy( input_buf_, "", sizeof( input_buf_ ) );
	}

	void Display() override {
		Camera *camera = GetApp()->gameManager->GetActiveCamera();
		ImGui::SetNextWindowSize( ImVec2( ( float ) ( camera->GetViewportWidth() ) - 20, 128 ), ImGuiCond_Once );
		ImGui::SetNextWindowPos( ImVec2( 10, ( float ) ( camera->GetViewportHeight() ) - 138 ) );
		ImGui::Begin( "Console", &show_console );
		if ( ImGui::InputText( "", input_buf_, 256, ImGuiInputTextFlags_EnterReturnsTrue )
		     && input_buf_[ 0 ] != '\0' ) {
			SendCommand();
		}
		ImGui::SameLine();
		if ( ImGui::Button( "Submit" ) ) {
			SendCommand();
		}
		ImGui::End();
	}

protected:
private:
	char input_buf_[256]{ '\0' };
};

void UI_DisplayDebugMenu( void ) {
	static bool show_about = false;
	if ( ImGui::BeginMainMenuBar() ) {
		if ( ImGui::BeginMenu( "File" ) ) {
			if ( ImGui::BeginMenu( "One Player" ) ) {
				if ( ImGui::MenuItem( "New Game" ) ) {
					windows.push_back( new NewGameWindow() );
				}
				if ( ImGui::MenuItem( "Load Game" ) ) {} // todo
				ImGui::EndMenu();
			}
			if ( ImGui::MenuItem( "Multi-Player" ) ) {}  // todo
			ImGui::Separator();
			if ( ImGui::MenuItem( "Create Map" ) ) {
				static NewMapWindow *popup = nullptr;
				if ( popup == nullptr ) {
					windows.push_back( ( popup = new NewMapWindow() ) );
				}
			}
			if ( ImGui::MenuItem( "Load Map" ) ) {} // todo
			ImGui::Separator();
			if ( ImGui::MenuItem( "Options" ) ) { show_settings = true; }
			if ( ImGui::MenuItem( "Controls" ) ) {} // todo
			ImGui::Separator();
			if ( ImGui::MenuItem( "Quit" ) ) {
				windows.push_back( new QuitWindow() );
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Debug" ) ) {
			static bool simStatus = false;
			if ( ImGui::MenuItem( "Toggle Simulation", "T", simStatus ) ) {
				GetApp()->gameManager->ToggleSimulation( ( simStatus = !simStatus ) );
			}
			if ( ImGui::MenuItem( "Step Simulation", "S" ) ) {
				GetApp()->gameManager->StepSimulation( 1 );
			}

			ImGui::Separator();

			if ( ImGui::MenuItem( "Show Console", "`" ) ) {
				windows.push_back( new ConsoleWindow() );
			}

#if 0
			static int tc = 0;
			if (ImGui::SliderInt("Show Texture Cache", &tc, 0, MAX_TEXTURE_INDEX)) {
			  char buf[4];
			  plSetConsoleVariable(cv_display_texture_cache, pl_itoa(tc - 1, buf, 4, 10));
			}

			if (ImGui::IsItemHovered() && tc > 0) {
			  const PLTexture *texture = Display_GetCachedTexture(
				  (unsigned int) cv_display_texture_cache->i_value);
			  if (texture != nullptr) {
				ImGui::BeginTooltip();
				ImGui::Image(reinterpret_cast<ImTextureID>(texture->internal.id),
							 ImVec2(texture->w, texture->h));
				ImGui::Text("%d (%dx%d)", cv_display_texture_cache->i_value, texture->w, texture->h);
				ImGui::EndTooltip();
			  }
			}
#endif

			ImGui::Separator();

			if ( ImGui::MenuItem( "Rebuild Shaders" ) ) {
				plParseConsoleString( "rebuildShaderPrograms" );
			}

			ImGui::Separator();

			if ( ImGui::BeginMenu( "Console Variables" ) ) {
				size_t num_c;
				PLConsoleVariable **vars;
				plGetConsoleVariables( &vars, &num_c );

				for ( PLConsoleVariable **var = vars; var < num_c + vars; ++var ) {
					switch ( ( *var )->type ) {
						case pl_float_var:
							if ( ImGui::InputFloat( ( *var )->var, &( *var )->f_value, 0, 10, nullptr,
							                        ImGuiInputTextFlags_EnterReturnsTrue ) ) {
								plSetConsoleVariable( ( *var ), std::to_string( ( *var )->f_value ).c_str() );
							}
							break;
						case pl_int_var:
							if ( ImGui::InputInt( ( *var )->var, &( *var )->i_value, 1, 10,
							                      ImGuiInputTextFlags_EnterReturnsTrue ) ) {
								plSetConsoleVariable( ( *var ), std::to_string( ( *var )->i_value ).c_str() );
							}
							break;
						case pl_string_var:
							// read-only for now
							ImGui::LabelText( ( *var )->var, "%s", ( *var )->s_value );
							break;
						case pl_bool_var:
							bool b = ( *var )->b_value;
							if ( ImGui::Checkbox( ( *var )->var, &b ) ) {
								plSetConsoleVariable( ( *var ), b ? "true" : "false" );
							}
							break;
					}

					if ( ImGui::IsItemHovered() ) {
						ImGui::BeginTooltip();
						ImGui::TextUnformatted( ( *var )->description );
						ImGui::EndTooltip();
					}
				}

				ImGui::EndMenu();
			}

			if ( ImGui::BeginMenu( "Console Commands" ) ) {
				size_t num;
				const PLConsoleCommand **cmds;
				plGetConsoleCommands( &cmds, &num );

				for ( const PLConsoleCommand **cmd = cmds; cmd < num + cmds; ++cmd ) {
					if ( ImGui::MenuItem( ( *cmd )->cmd ) ) {
						plParseConsoleString( ( *cmd )->cmd );
					}

					if ( ImGui::IsItemHovered() ) {
						ImGui::BeginTooltip();
						ImGui::TextUnformatted( ( *cmd )->description );
						ImGui::EndTooltip();
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		// todo: eventually this will be moved into a dedicated toolbar
		if ( ImGui::BeginMenu( "Tools" ) ) {
			if ( ImGui::MenuItem( "Particle Editor..." ) ) {
				windows.push_back( new ParticleEditor() );
			}
			if ( GetApp()->gameManager->IsModeActive() ) {
				if ( ImGui::MenuItem( "Import Heightmap..." ) ) {
					windows.push_back( new TerrainImportWindow() );
				}
				ImGui::Separator();
				if ( ImGui::MenuItem( "Actor Inspector..." ) ) { windows.push_back( new ActorTreeWindow() ); }
				if ( ImGui::MenuItem( "Map Config Editor..." ) ) { windows.push_back( new MapConfigEditor() ); }
				if ( ImGui::MenuItem( "Texture Picker..." ) ) { windows.push_back( new TexturePicker() ); }
			}
			if ( ImGui::MenuItem( "Model Viewer..." ) ) { windows.push_back( new ModelViewer() ); }
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Help" ) ) {
			ImGui::MenuItem( "About OpenHoW", nullptr, &show_about );
			ImGui::EndMenu();
		}

		ImGui::Separator();

		unsigned int fps, ms;
		Display_GetFramesCount( &fps, &ms );

		ImGui::PushItemWidth( 64 );
		ImGui::Text( "FPS %d (%dms)", fps, ms );
		ImGui::PopItemWidth();

		ImGui::EndMainMenuBar();
	}

	// Prompt to notify players that games can be started
	// via File > New Game, until we get an actual frontend
	// implementation done.
	if ( !GetApp()->gameManager->IsModeActive() ) {
		ImGui::SetNextWindowSize( ImVec2( 320, 128 ) );
		ImGui::SetNextWindowPos( ImVec2(
				static_cast<float>(cv_display_width->i_value) * 0.5f,
				static_cast<float>(cv_display_height->i_value) * 0.5f ),
		                         ImGuiCond_Always,
		                         ImVec2( 0.5f, 0.5f ) );
		static bool state = false;
		if ( ImGui::Begin(
				"InstructionMenu",
				&state,
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoInputs |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoBackground
		) ) {
			ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
			ImGui::TextWrapped( "%s", lm_gtr( "$dbt0" ) );
			ImGui::PopStyleColor();
			ImGui::TextWrapped( "%s", lm_gtr( "$dbt1" ) );
			ImGui::Text( "%s", lm_gtr( "$dbt2" ) );
			ImGui::End();
		}
	}

	for ( auto window = windows.begin(); window != windows.end(); ) {
		if ( ( *window )->GetStatus() ) {
			( *window )->Display();
			++window;
			continue;
		}

		delete ( *window );
		window = windows.erase( window );
	}
}

void ImGuiImpl_RegisterWindow( BaseWindow *window ) {
	windows.push_back( window );
}

bool ImGuiImpl_HandleEvent( const SDL_Event &event ) {
	ImGuiIO &io = ImGui::GetIO();

	if ( !io.WantCaptureKeyboard && !io.WantCaptureMouse ) {
		return false;
	}

	switch ( event.type ) {
		case SDL_KEYUP:
		case SDL_KEYDOWN: {
			if ( !io.WantCaptureKeyboard ) {
				break;
			}

			// Always update if key-up; see
			// https://github.com/TalonBraveInfo/OpenHoW/issues/70#issuecomment-507377604
			int key = event.key.keysym.scancode;
			IM_ASSERT( key >= 0 && key < IM_ARRAYSIZE( io.KeysDown ) );
			io.KeysDown[ key ] = ( event.type == SDL_KEYDOWN );
			io.KeyShift = ( ( SDL_GetModState() & KMOD_SHIFT ) != 0 );
			io.KeyCtrl = ( ( SDL_GetModState() & KMOD_CTRL ) != 0 );
			io.KeyAlt = ( ( SDL_GetModState() & KMOD_ALT ) != 0 );
			io.KeySuper = ( ( SDL_GetModState() & KMOD_GUI ) != 0 );
			if ( event.type != SDL_KEYUP ) {
				return true;
			}

			break;
		}

		case SDL_TEXTINPUT: {
			if ( !io.WantCaptureKeyboard ) {
				break;
			}

			io.AddInputCharactersUTF8( event.text.text );
			return true;
		}

		case SDL_MOUSEWHEEL: {
			if ( !io.WantCaptureMouse ) {
				break;
			}

			if ( event.wheel.x > 0 ) {
				io.MouseWheelH += 1;
			} else if ( event.wheel.x < 0 ) {
				io.MouseWheelH -= 1;
			}

			if ( event.wheel.y > 0 ) {
				io.MouseWheel += 1;
			} else if ( event.wheel.y < 0 ) {
				io.MouseWheel -= 1;
			}

			return true;
		}

		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN: {
			if ( !io.WantCaptureMouse ) {
				break;
			}

			switch ( event.button.button ) {
				case SDL_BUTTON_LEFT: {
					io.MouseDown[ 0 ] = event.button.state;
					break;
				}
				case SDL_BUTTON_RIGHT: {
					io.MouseDown[ 1 ] = event.button.state;
					break;
				}
				case SDL_BUTTON_MIDDLE: {
					io.MouseDown[ 2 ] = event.button.state;
					break;
				}

				default:
					break;
			}

			return true;
		}

		case SDL_MOUSEMOTION: {
			io.MousePos.x = event.motion.x;
			io.MousePos.y = event.motion.y;
			break;
		}

		case SDL_WINDOWEVENT: {
			if ( event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) {
				imguiCamera->viewport.w = event.window.data1;
				imguiCamera->viewport.h = event.window.data2;
				io.DisplaySize = ImVec2( event.window.data1, event.window.data2 );
			}
			break;
		}
	}

	return false;
}
