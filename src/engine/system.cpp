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

#include "engine.h"
#include "input.h"
#include "imgui_layer.h"

#include "../3rdparty/imgui/examples/imgui_impl_sdl.h"
#include "../3rdparty/imgui/examples/imgui_impl_opengl3.h"

#include "graphics/display.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
/* kill windows forever pls */
#undef near
#undef far

static SDL_Window *window = nullptr;
static SDL_GLContext gl_context = nullptr;

using namespace openhow;

unsigned int System_GetTicks(void) {
  return SDL_GetTicks();
}

void System_DisplayMessageBox(unsigned int level, const char *msg, ...) {
  switch (level) {
    case PROMPT_LEVEL_ERROR: {
      level = SDL_MESSAGEBOX_ERROR;
    }
      break;
    case PROMPT_LEVEL_WARNING: {
      level = SDL_MESSAGEBOX_WARNING;
    }
      break;
    case PROMPT_LEVEL_DEFAULT: {
      level = SDL_MESSAGEBOX_INFORMATION;
    }
      break;

    default: return;
  }

  char buf[2048];
  va_list args;
  va_start(args, msg);
  vsnprintf(buf, sizeof(buf), msg, args);
  va_end(args);

  SDL_ShowSimpleMessageBox(level, ENGINE_TITLE, buf, window);
}

const char *System_GetClipboardText(void *) {
  static char *clipboard = nullptr;
  if (clipboard != nullptr) {
    SDL_free(clipboard);
  }

  clipboard = SDL_GetClipboardText();
  return clipboard;
}

void System_SetClipboardText(void *, const char *text) {
  SDL_SetClipboardText(text);
}

int System_SetSwapInterval(int interval) {
  if(SDL_GL_SetSwapInterval(interval) != 0) {
    LogWarn("Failed to set desired swap interval (%d)!\n", interval);
  }

  return SDL_GL_GetSwapInterval();
}

void System_DisplayWindow(bool fullscreen, int width, int height) {
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

#ifdef _DEBUG
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  unsigned int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS;
  if (fullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN;
  } else {
    flags |= SDL_WINDOW_SHOWN;
  }

  window = SDL_CreateWindow(
      ENGINE_TITLE,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      width, height,
      flags
  );
  if (window == nullptr) {
    System_DisplayMessageBox(PROMPT_LEVEL_ERROR, "Failed to create window!\n%s", SDL_GetError());
    System_Shutdown();
  }

  {
    //Gen list of video presets
    VideoPreset tmp_preset;
    int num_display_modes = SDL_GetNumDisplayModes(0);
    if (num_display_modes > 0) {
      LogInfo("Generating video presets from %d display modes", num_display_modes);
      for (int i = 0; i < num_display_modes; ++i) {
        SDL_DisplayMode tmp_mode;
        if (SDL_GetDisplayMode(0, i, &tmp_mode) != 0) {
          LogInfo("Failed to get an SDL display mode: %s", SDL_GetError());
          continue;
        }

        //Presets enumerated in order, avoid duplicates with differing refresh rates
        if (tmp_preset.width != tmp_mode.w || tmp_preset.height != tmp_mode.h) {
          Display_AppendVideoPreset(tmp_mode.w, tmp_mode.h);
          tmp_preset.width = tmp_mode.w;
          tmp_preset.height = tmp_mode.h;
        }
      }
    } else {
      LogInfo("No display modes founds, failed to generate video presets");
    }
  }

  SDL_SetWindowMinimumSize(window, MIN_DISPLAY_WIDTH, MIN_DISPLAY_HEIGHT);

  if ((gl_context = SDL_GL_CreateContext(window)) == nullptr) {
    System_DisplayMessageBox(PROMPT_LEVEL_ERROR, "Failed to create context!\n%s", SDL_GetError());
    System_Shutdown();
  }

  /* setup imgui integration */

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();

  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  //io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

  io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
  io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
  io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
  io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
  io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
  io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
  io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
  io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
  io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
  io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
  io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
  io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
  io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
  io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
  io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
  io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

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

  // https://github.com/ocornut/imgui/issues/707#issuecomment-468798935
  {
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    /// 0 = FLAT APPEARENCE
    /// 1 = MORE "3D" LOOK
    float is3D = 1;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
    colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
    colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

    style.PopupRounding = 3;

    style.WindowPadding = ImVec2(4, 4);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(6, 2);

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

  io.Fonts->Clear();
  if (io.Fonts->AddFontFromFileTTF("fonts/OpenSans-SemiBold.ttf", 16.f, nullptr, nullptr) == nullptr) {
    io.Fonts->AddFontDefault();
  }
}

void System_SetWindowTitle(const char *title) {
  SDL_SetWindowTitle(window, title);
}

void System_GetWindowSize(int *width, int *height, bool *fs) {
  SDL_GetWindowSize(window, width, height);
  *fs = static_cast<bool>(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
}

void System_GetWindowDrawableSize(int *width, int *height, bool *fs) {
  SDL_GL_GetDrawableSize(window, width, height);
  *fs = static_cast<bool>(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
}

bool System_SetWindowSize(int width, int height, bool fs) {
  //Kick window out of minimized / maximized mode when resizing
  SDL_SetWindowFullscreen(window, 0);
  SDL_RestoreWindow(window);

  //Ensure the size_changed event is triggered, this will auto-maximise window when width/height are greater than the desktop dimensions
  SDL_SetWindowSize(window, MIN_DISPLAY_WIDTH, MIN_DISPLAY_HEIGHT);
  SDL_SetWindowSize(window, width, height);

  if (fs) {
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  }

  //Update console values
  char buf[16];
  plSetConsoleVariable(cv_display_width, pl_itoa(width, buf, 16, 10));
  plSetConsoleVariable(cv_display_height, pl_itoa(height, buf, 16, 10));
  plSetConsoleVariable(cv_display_fullscreen, fs ? "true" : "false");

  return true;
}

void System_SwapDisplay(void) {
  SDL_GL_SwapWindow(window);
}

void System_Shutdown(void) {
  delete openhow::engine;

  ImGui_ImplOpenGL3_DestroyDeviceObjects();
  ImGui::DestroyContext();

  SDL_StopTextInput();

  if (gl_context != nullptr) {
    SDL_GL_DeleteContext(gl_context);
  }

  if (window != nullptr) {
    SDL_DestroyWindow(window);
  }

  SDL_ShowCursor(SDL_TRUE);

  SDL_EnableScreenSaver();
  SDL_Quit();

  plShutdown();

  exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////

static int TranslateSDLKey(int key) {
  if (key < 128) {
    return key;
  }

  switch (key) {
    case SDLK_F1: return INPUT_KEY_F1;
    case SDLK_F2: return INPUT_KEY_F2;
    case SDLK_F3: return INPUT_KEY_F3;
    case SDLK_F4: return INPUT_KEY_F4;
    case SDLK_F5: return INPUT_KEY_F5;
    case SDLK_F6: return INPUT_KEY_F6;
    case SDLK_F7: return INPUT_KEY_F7;
    case SDLK_F8: return INPUT_KEY_F8;
    case SDLK_F9: return INPUT_KEY_F9;
    case SDLK_F10: return INPUT_KEY_F10;
    case SDLK_F11: return INPUT_KEY_F11;
    case SDLK_F12: return INPUT_KEY_F12;

    case SDLK_ESCAPE: return INPUT_KEY_ESCAPE;

    case SDLK_PAUSE: return INPUT_KEY_PAUSE;
    case SDLK_INSERT: return INPUT_KEY_INSERT;
    case SDLK_HOME: return INPUT_KEY_HOME;

    case SDLK_UP: return INPUT_KEY_UP;
    case SDLK_DOWN: return INPUT_KEY_DOWN;
    case SDLK_LEFT: return INPUT_KEY_LEFT;
    case SDLK_RIGHT: return INPUT_KEY_RIGHT;

    case SDLK_SPACE: return INPUT_KEY_SPACE;

    case SDLK_LSHIFT: return INPUT_KEY_LSHIFT;
    case SDLK_RSHIFT: return INPUT_KEY_RSHIFT;

    case SDLK_PAGEUP: return INPUT_KEY_PAGEUP;
    case SDLK_PAGEDOWN: return INPUT_KEY_PAGEDOWN;

    default: return -1;
  }
}

static int TranslateSDLMouseButton(int button) {
  switch (button) {
    case SDL_BUTTON_LEFT: return INPUT_MOUSE_BUTTON_LEFT;
    case SDL_BUTTON_RIGHT: return INPUT_MOUSE_BUTTON_RIGHT;
    case SDL_BUTTON_MIDDLE: return INPUT_MOUSE_BUTTON_MIDDLE;
    default: return -1;
  }
}

static int TranslateSDLButton(int button) {
  switch (button) {
    case SDL_CONTROLLER_BUTTON_A: return INPUT_BUTTON_CROSS;
    case SDL_CONTROLLER_BUTTON_B: return INPUT_BUTTON_CIRCLE;
    case SDL_CONTROLLER_BUTTON_X: return INPUT_BUTTON_SQUARE;
    case SDL_CONTROLLER_BUTTON_Y: return INPUT_BUTTON_TRIANGLE;

    case SDL_CONTROLLER_BUTTON_BACK: return INPUT_BUTTON_SELECT;
    case SDL_CONTROLLER_BUTTON_START: return INPUT_BUTTON_START;

    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return INPUT_BUTTON_L1;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK: return INPUT_BUTTON_L3;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return INPUT_BUTTON_R1;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return INPUT_BUTTON_R3;

    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return INPUT_BUTTON_DOWN;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return INPUT_BUTTON_LEFT;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return INPUT_BUTTON_RIGHT;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: return INPUT_BUTTON_UP;

    default: return -1;
  }
}

void System_PollEvents() {
  ImGuiIO &io = ImGui::GetIO();

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      default:break;

      case SDL_KEYUP:
      case SDL_KEYDOWN: {
        // Always update if key-up; see
        // https://github.com/TalonBraveInfo/OpenHoW/issues/70#issuecomment-507377604
        if (event.type == SDL_KEYUP || io.WantCaptureKeyboard) {
          int key = event.key.keysym.scancode;
          IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
          io.KeysDown[key] = (event.type == SDL_KEYDOWN);
          io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
          io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
          io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
          io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
          if (event.type != SDL_KEYUP) break;
        }

        int key = TranslateSDLKey(event.key.keysym.sym);
        if (key != -1) {
          Input_SetKeyState(key, (event.type == SDL_KEYDOWN));
        }
      }
        break;

      case SDL_TEXTINPUT: {
        if (io.WantCaptureKeyboard) {
          io.AddInputCharactersUTF8(event.text.text);
          break;
        }

        Input_AddTextCharacter(event.text.text);
      }
        break;

      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN: {
        if (io.WantCaptureMouse) {
          switch (event.button.button) {
            case SDL_BUTTON_LEFT:io.MouseDown[0] = event.button.state;
              break;
            case SDL_BUTTON_RIGHT:io.MouseDown[1] = event.button.state;
              break;
            case SDL_BUTTON_MIDDLE:io.MouseDown[2] = event.button.state;
              break;

            default:break;
          }

          break;
        }

        int button = TranslateSDLMouseButton(event.button.button);
        Input_SetMouseState(event.motion.x, event.motion.y, button, event.button.state);
      }
        break;

      case SDL_MOUSEWHEEL: {
        if (event.wheel.x > 0) {
          io.MouseWheelH += 1;
        } else if (event.wheel.x < 0) {
          io.MouseWheelH -= 1;
        }

        if (event.wheel.y > 0) {
          io.MouseWheel += 1;
        } else if (event.wheel.y < 0) {
          io.MouseWheel -= 1;
        }
      }
        break;

      case SDL_MOUSEMOTION: {
        io.MousePos.x = event.motion.x;
        io.MousePos.y = event.motion.y;

        Input_SetMouseState(event.motion.x, event.motion.y, -1, false);
      }
        break;

      case SDL_CONTROLLERBUTTONUP: {
        int button = TranslateSDLButton(event.cbutton.button);
        if (button != -1) {
          Input_SetButtonState((unsigned int) event.cbutton.which, button, false);
        }
      }
        break;

      case SDL_CONTROLLERBUTTONDOWN: {
        int button = TranslateSDLButton(event.cbutton.button);
        if (button != -1) {
          Input_SetButtonState((unsigned int) event.cbutton.which, button, true);
        }
      }
        break;

      case SDL_CONTROLLERAXISMOTION: {
        if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.value > 1000) {
          Input_SetButtonState((unsigned int) event.caxis.which, INPUT_BUTTON_L2, true);
        } else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.value <= 1000) {
          Input_SetButtonState((unsigned int) event.caxis.which, INPUT_BUTTON_L2, false);
        }

        if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && event.caxis.value > 1000) {
          Input_SetButtonState((unsigned int) event.caxis.which, INPUT_BUTTON_R2, true);
        } else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && event.caxis.value <= 1000) {
          Input_SetButtonState((unsigned int) event.caxis.which, INPUT_BUTTON_R2, false);
        }

        Input_SetAxisState(event.caxis.which, event.caxis.axis, event.caxis.value);
      }
        break;

      case SDL_QUIT: {
        System_Shutdown();
      }
        break;

      case SDL_WINDOWEVENT: {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          char buf[16];
          plSetConsoleVariable(cv_display_width, pl_itoa((unsigned int) event.window.data1, buf, 16, 10));
          plSetConsoleVariable(cv_display_height, pl_itoa((unsigned int) event.window.data2, buf, 16, 10));
          Display_UpdateViewport(0, 0, (unsigned int) event.window.data1, (unsigned int) event.window.data2);
          ImGuiImpl_UpdateViewport(event.window.data1, event.window.data2);
          io.DisplaySize = ImVec2(event.window.data1, event.window.data2);
        }
      }
        break;
    }
  }
}

static void *u_malloc(size_t size) {
  return u_alloc(1, size, true);
}

static void *u_calloc(size_t num, size_t size) {
  return u_alloc(num, size, true);
}

int main(int argc, char **argv) {
  pl_malloc = u_malloc;
  pl_calloc = u_calloc;

  plInitialize(argc, argv);

  char app_dir[PL_SYSTEM_MAX_PATH];
  plGetApplicationDataDirectory(ENGINE_APP_NAME, app_dir, PL_SYSTEM_MAX_PATH);

  if (!plCreatePath(app_dir)) {
    System_DisplayMessageBox(PROMPT_LEVEL_WARNING,
                             "Unable to create %s: %s\n"
                             "Settings will not be saved.", app_dir, plGetError());
  }

  std::string log_path = std::string(app_dir) + "/" + ENGINE_LOG;
  u_init_logs(log_path.c_str());

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    System_DisplayMessageBox(PROMPT_LEVEL_ERROR, "Failed to initialize SDL2!\n%s", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_DisableScreenSaver();

  //SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_CaptureMouse(SDL_TRUE);
  SDL_ShowCursor(SDL_TRUE);

  /* using this to catch modified keys
   * without having to do the conversion
   * ourselves                            */
  SDL_StartTextInput();

  engine = new Engine();
  engine->Initialize();

  while (engine->IsRunning());

  System_Shutdown();

  return EXIT_SUCCESS;
}
