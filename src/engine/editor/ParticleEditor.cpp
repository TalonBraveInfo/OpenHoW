// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "graphics/ShaderManager.h"
#include "graphics/ParticleEffect.h"
#include "graphics/ParticleEmitter.h"
#include "graphics/Camera.h"
#include "Language.h"
#include "imgui_layer.h"

#include "ParticleEditor.h"

#define VIEWER_WIDTH  640
#define VIEWER_HEIGHT 480

std::list< std::string > ohw::ParticleEditor::particleList;

ohw::ParticleEditor::ParticleEditor() : BaseWindow() {
	camera = new Camera( { -2500.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );
	camera->SetViewport( 0, 0, VIEWER_WIDTH, VIEWER_HEIGHT );

	GenerateFrameBuffer( VIEWER_WIDTH, VIEWER_HEIGHT );
}

ohw::ParticleEditor::~ParticleEditor() {
	delete particleEffect;

	PlgDestroyTexture( textureAttachment );
	PlgDestroyFrameBuffer( drawBuffer );
}

void ohw::ParticleEditor::DrawViewport() {
	PlgBindFrameBuffer( drawBuffer, PLG_FRAMEBUFFER_DRAW );

	PlgSetDepthBufferMode( PLG_DEPTHBUFFER_ENABLE );

	PlgSetClearColour( { 0, 0, 0, 0 } );
	PlgClearBuffers( PLG_BUFFER_COLOUR | PLG_BUFFER_DEPTH );

	unsigned int width, height;
	PlgGetFrameBufferResolution( drawBuffer, &width, &height );

	camera->SetViewport( 0, 0, width, height );
	camera->MakeActive();

	hei::Vector3 angles(
			PlDegreesToRadians( modelRotation.x ),
			PlDegreesToRadians( modelRotation.y ),
			PlDegreesToRadians( modelRotation.z ) );

	if ( viewGrid ) {
		ShaderProgram *shaderProgram = Shaders_GetProgram( "generic_untextured" );
		shaderProgram->Enable();

		PlMatrixMode( PL_MODELVIEW_MATRIX );
		PlPushMatrix();
		PlLoadIdentityMatrix();

		PlRotateMatrix( PlDegreesToRadians( modelRotation.z + 90.0f ), 1, 0, 0 );
		PlRotateMatrix( angles.y, 0, 1, 0 );
		PlRotateMatrix( angles.x, 0, 0, 1 );

		PlgDrawGrid( -512, -512, 1024, 1024, 32 );

		PlPopMatrix();
	}

	if ( particleEffect == nullptr ) {
		// Fallback to default
		PlgBindFrameBuffer( nullptr, PLG_FRAMEBUFFER_DRAW );
		return;
	}
}

void ohw::ParticleEditor::Display() {
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

				for ( const auto &i: particleList ) {
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

void ohw::ParticleEditor::GenerateFrameBuffer( unsigned int width, unsigned int height ) {
	unsigned int bufferWidth = 0, bufferHeight = 0;
	if ( drawBuffer != nullptr ) {
		PlgGetFrameBufferResolution( drawBuffer, &bufferWidth, &bufferHeight );
	}

	if ( bufferWidth != width || bufferHeight != height ) {
		PlgDestroyFrameBuffer( drawBuffer );
		drawBuffer = PlgCreateFrameBuffer( width, height, PLG_BUFFER_COLOUR | PLG_BUFFER_DEPTH );
		if ( drawBuffer == nullptr ) {
			Error( "Failed to create framebuffer for model viewer (%s)!\n", PlGetError() );
		}

		PlgDestroyTexture( textureAttachment );
		textureAttachment = PlgGetFrameBufferTextureAttachment( drawBuffer, PLG_BUFFER_COLOUR, PLG_TEXTURE_FILTER_LINEAR );
		if ( textureAttachment == nullptr ) {
			Error( "Failed to create texture attachment for buffer (%s)!\n", PlGetError() );
		}
	}

	PlgBindFrameBuffer( nullptr, PLG_FRAMEBUFFER_DRAW );
}
