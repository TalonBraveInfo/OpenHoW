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
#include "TexturePicker.h"
#include "language.h"
#include "imgui_layer.h"

ohw::TexturePicker::TexturePicker() : BaseWindow() {
	std::string mapDir = Engine::Game()->GetCurrentMapDirectory();
	if ( mapDir.empty() ) {
		return;
	}

	mapDir.append( "tiles/" );

	unsigned int numFormats;
	const char **formatExtensions = plGetSupportedImageFormats( &numFormats );

	for ( unsigned int i = 0;; ++i ) {
		bool indexLoaded = false;
		for ( unsigned int j = 0; j < numFormats; ++j ) {
			std::string texturePath = mapDir + std::to_string( i ) + "." + formatExtensions[ j ];
			if ( plFileExists( texturePath.c_str() ) ) {
				SharedTextureResourcePointer sharedTexture = Engine::Resource()->LoadTexture(
						texturePath,
						TextureResource::FLAG_NOMIPS | TextureResource::FLAG_NEAREST
				);
				textures.push_back( sharedTexture );
				indexLoaded = true;
				break;
			}
		}

		// Failed to load the index? Assume it's the end. (let's do this better in future)
		if ( !indexLoaded ) {
			LogInfo( "Didn't find texture index %d. Assuming end of tiles list!\n", i );
			break;
		}
	}
}

ohw::TexturePicker::~TexturePicker() {
	textures.clear();
}

void ohw::TexturePicker::Display() {
	ImGui::SetNextWindowSize( ImVec2( 348, 512 ), ImGuiCond_Once );

	unsigned int flags =
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;
	Begin( "Texture Picker", flags );

	if ( ImGui::BeginMenuBar() ) {
		if ( ImGui::BeginMenu( "Sets" ) ) {
#if 0
			static ImGuiTextFilter filter;
			filter.Draw();

			// todo: provide a list of all available tile sets
#endif
			ImGui::EndMenu();
		}

		ImGui::Separator();

		ImGui::Text( "Set: %s", "Blah" );
		ImGui::Text( "Tiles: %u", ( unsigned int ) textures.size() );

		ImGui::EndMenuBar();
	}

#define TEXTURES_PER_ROW 4
	for ( unsigned int i = 0, j = 0; i < textures.size(); ++i, ++j ) {
		if ( j >= TEXTURES_PER_ROW ) {
			ImGui::NewLine();
			j = 0;
		}

		ImTextureID textureId = reinterpret_cast< ImTextureID >(textures[ i ]->GetInternalTexture()->internal.id);

		ImGui::ImageButton( textureId, ImVec2( 64, 64 ) );
		ImGui::SameLine();
	}

	ImGui::End();
}
