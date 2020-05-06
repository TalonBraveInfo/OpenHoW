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

#include "../engine.h"
#include "../graphics/shaders.h"
#include "../graphics/display.h"
#include "../graphics/particles.h"
#include "../language.h"
#include "../imgui_layer.h"

#include "window_particle_editor.h"

#define VIEWER_WIDTH  640
#define VIEWER_HEIGHT 480

std::list< std::string > ParticleEditor::particleList;

ParticleEditor::ParticleEditor() : BaseWindow() {
	camera = new Camera( { -2500.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );
	camera->SetViewport( 0, 0, 640, 480 );

	GenerateFrameBuffer( 640, 480 );
}

ParticleEditor::~ParticleEditor() {
	delete particleEffect;

	plDestroyTexture( textureAttachment );
	plDestroyFrameBuffer( drawBuffer );
}

void ParticleEditor::DrawViewport() {

}

void ParticleEditor::Display() {
	DrawViewport();

	unsigned int flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize( ImVec2( VIEWER_WIDTH, VIEWER_HEIGHT ), ImGuiCond_Once );

	Begin( lm_gtr( "$window_particle_editor" ), flags );

	if ( ImGui::BeginMenuBar() ) {
		if ( ImGui::BeginMenu( lm_gtr( "$file" ) ) ) {
			if ( ImGui::BeginMenu( lm_gtr( "$open" ) ) ) {
				static ImGuiTextFilter filter;
				filter.Draw();

				for ( const auto &i : particleList ) {
					if ( !filter.PassFilter( i.c_str() ) ) {
						continue;
					}

					bool selected = false;
					if ( particleEffect != nullptr ) {
						selected = ( i == particleEffect->GetPath() );
					}

					if ( ImGui::Selectable( i.c_str(), selected ) ) {
						delete particleEffect;
						particleEffect = nullptr;

#if 0
						modelPtr = plLoadModel( i.c_str() );
						if ( modelPtr == nullptr ) {
							LogWarn( "Failed to load \"%s\" (%s)!\n", i.c_str(), plGetError() );
						}
#endif
					}
				}

				ImGui::EndMenu();
			}
		}
	}

	ImGui::End();
}

void ParticleEditor::GenerateFrameBuffer( unsigned int width, unsigned int height ) {
	unsigned int bufferWidth = 0, bufferHeight = 0;
	if ( drawBuffer != nullptr ) {
		plGetFrameBufferResolution( drawBuffer, &bufferWidth, &bufferHeight );
	}

	if ( bufferWidth != width || bufferHeight != height ) {
		plDestroyFrameBuffer( drawBuffer );
		drawBuffer = plCreateFrameBuffer( width, height, PL_BUFFER_COLOUR | PL_BUFFER_DEPTH );
		if ( drawBuffer == nullptr ) {
			Error( "Failed to create framebuffer for model viewer (%s)!\n", plGetError() );
		}

		plDestroyTexture( textureAttachment );
		textureAttachment = plGetFrameBufferTextureAttachment( drawBuffer );
		if ( textureAttachment == nullptr ) {
			Error( "Failed to create texture attachment for buffer (%s)!\n", plGetError() );
		}
	}

	plBindFrameBuffer( nullptr, PL_FRAMEBUFFER_DRAW );
}
