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

#include "base_window.h"

class TextureViewer : public BaseWindow {
public:
	TextureViewer( PLTexture *texture ) : BaseWindow() {
		texturePtr = texture;
	}

	TextureViewer( const std::string &path ) {
		texturePtr = openhow::Engine::Resource()->LoadTexture( path, PL_TEXTURE_FILTER_LINEAR );
		if ( texturePtr == nullptr ) {
			throw std::runtime_error( "Failed to load specified texture, \"" + path + "\" (" + plGetError() + ")!" );
		}

		texturePath = path;
	}

	~TextureViewer() override = default;

	void ReloadTexture( PLTextureFilter filter_mode ) {
		if ( texturePath.empty() || filter_mode == filterMode ) {
			return;
		}

		plDestroyTexture( texturePtr );
		texturePtr = openhow::Engine::Resource()->LoadTexture( texturePath, filter_mode );
		filterMode = filter_mode;
	}

	void Display() override {
		ImGui::SetNextWindowSize( ImVec2( texturePtr->w + 64, texturePtr->h + 128 ), ImGuiCond_Once );
		Begin( "Texture Viewer",
			   ImGuiWindowFlags_MenuBar |
				   ImGuiWindowFlags_HorizontalScrollbar |
				   ImGuiWindowFlags_NoSavedSettings
		);

		if ( ImGui::BeginMenuBar() ) {
			if ( ImGui::BeginMenu( "View" ) ) {
				ImGui::SliderInt( "Scale", &scale_, 1, 8 );
				if ( ImGui::BeginMenu( "Filter Mode" ) ) {
					if ( ImGui::MenuItem( "Linear", nullptr, ( filterMode == PL_TEXTURE_FILTER_LINEAR ) ) ) {
						ReloadTexture( PL_TEXTURE_FILTER_LINEAR );
					}
					if ( ImGui::MenuItem( "Nearest", nullptr, ( filterMode == PL_TEXTURE_FILTER_NEAREST ) ) ) {
						ReloadTexture( PL_TEXTURE_FILTER_NEAREST );
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Image( reinterpret_cast<ImTextureID>(texturePtr->internal.id), ImVec2(
			texturePtr->w * scale_, texturePtr->h * scale_ ) );
		ImGui::Separator();
		ImGui::Text( "Path: %s", texturePath.c_str() );
		ImGui::Text( "%dx%d", texturePtr->w, texturePtr->h );
		ImGui::Text( "Size: %ukB (%luB)", ( unsigned int ) plBytesToKilobytes( texturePtr->size ),
					 ( long unsigned ) texturePtr->size );

		ImGui::End();
	}

protected:
private:
	PLTexture *texturePtr{ nullptr };
	std::string texturePath;

	PLTextureFilter filterMode{ PL_TEXTURE_FILTER_LINEAR };

	int scale_{ 1 };
};
