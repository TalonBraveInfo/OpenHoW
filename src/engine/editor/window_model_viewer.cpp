/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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
#include "../imgui_layer.h"
#include "../model.h"

#include "window_model_viewer.h"
#include "window_texture_viewer.h"

#define VIEWER_WIDTH  640
#define VIEWER_HEIGHT 480

std::list<std::string> ModelViewer::modelList;

ModelViewer::ModelViewer() : BaseWindow() {
	const char **formatExtensions = supported_model_formats;
	while ( *formatExtensions != nullptr ) {
		plScanDirectory( "chars", *formatExtensions, &ModelViewer::AppendModelList, true );
		formatExtensions++;
	}

	modelList.sort();
	modelList.unique();

	camera = new Camera( { -2500.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );
	camera->SetViewport( { 0, 0 }, { 640, 480 } );

	GenerateFrameBuffer( 640, 480 );
}

ModelViewer::~ModelViewer() {
	plDestroyModel( modelPtr );
	plDestroyTexture( textureAttachment );
	plDestroyFrameBuffer( drawBuffer );
}

void ModelViewer::DrawViewport() {
	plBindFrameBuffer( drawBuffer, PL_FRAMEBUFFER_DRAW );

	plSetDepthBufferMode( PL_DEPTHBUFFER_ENABLE );

	plSetClearColour( { 0, 0, 0, 0 } );
	plClearBuffers( PL_BUFFER_COLOUR | PL_BUFFER_DEPTH );

	unsigned int bufferWidth = 0, bufferHeight = 0;
	plGetFrameBufferResolution( drawBuffer, &bufferWidth, &bufferHeight );
	camera->SetViewport( { 0, 0 }, { bufferWidth, bufferHeight } );
	camera->MakeActive();

	PLVector3 angles(
		plDegreesToRadians( modelRotation.x ),
		plDegreesToRadians( modelRotation.y ),
		plDegreesToRadians( modelRotation.z ) );

	if ( viewGrid ) {
		ShaderProgram *shaderProgram = Shaders_GetProgram( "generic_untextured" );
		shaderProgram->Enable();

		PLMatrix4 matrix;
		matrix.Identity();
		matrix.Rotate( plDegreesToRadians( modelRotation.z + 90.0f ), { 1, 0, 0 } );
		matrix.Rotate( angles.y, { 0, 1, 0 } );
		matrix.Rotate( angles.x, { 0, 0, 1 } );

		plDrawGrid( &matrix, -512, -512, 1024, 1024, 32 );
	}

	if ( modelPtr == nullptr ) {
		plBindFrameBuffer( nullptr, PL_FRAMEBUFFER_DRAW );
		return;
	}

	ShaderProgram *shaderProgram = Shaders_GetProgram( viewDebugNormals ? "debug_normals" : "generic_textured_lit" );
	shaderProgram->Enable();

	if ( !viewDebugNormals ) {
		plSetNamedShaderUniformVector3( shaderProgram->GetInternalProgram(), "sun_position",
										PLVector3( 0.5f, 0.2f, 0.6f ) );
		plSetNamedShaderUniformVector4( shaderProgram->GetInternalProgram(), "sun_colour",
										PLColour( 255, 255, 255, 255 ).ToVec4() );
		plSetNamedShaderUniformVector4( shaderProgram->GetInternalProgram(), "ambient_colour",
										PLColour( 128, 128, 128, 255 ).ToVec4() );
	}

	if ( viewRotate ) {
		modelRotation.y += 0.01f * g_state.last_draw_ms;
	}

	PLMatrix4 *matrixPtr = &modelPtr->model_matrix;
	matrixPtr->Identity();
	matrixPtr->Rotate( angles.z, { 1, 0, 0 } );
	matrixPtr->Rotate( angles.y, { 0, 1, 0 } );
	matrixPtr->Rotate( angles.x, { 0, 0, 1 } );

	plDrawModel( modelPtr );

	plBindFrameBuffer( nullptr, PL_FRAMEBUFFER_DRAW );
}

void ModelViewer::Display() {
	// Draw the model view if there's a valid model
	DrawViewport();

	unsigned int flags =
		ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize( ImVec2( VIEWER_WIDTH, VIEWER_HEIGHT ), ImGuiCond_Once );

	Begin( "Model Viewer", flags );
	if ( ImGui::BeginMenuBar() ) {
		ImGui::EndMenuBar();
	}

	if ( ImGui::BeginMenuBar() ) {
		if ( ImGui::BeginMenu( "Models" ) ) {
			static ImGuiTextFilter filter;
			filter.Draw();

			for ( const auto &i : modelList ) {
				if ( !filter.PassFilter( i.c_str() ) ) {
					continue;
				}

				bool selected = false;
				if ( modelPtr != nullptr ) {
					selected = ( i == modelPtr->path );
				}

				if ( ImGui::Selectable( i.c_str(), selected ) ) {
					plDestroyModel( modelPtr );
					modelPtr = nullptr;

					modelPtr = plLoadModel( i.c_str() );
					if ( modelPtr == nullptr ) {
						LogWarn( "Failed to load \"%s\" (%s)!\n", i.c_str(), plGetError() );
					}
				}
			}
			ImGui::EndMenu();
		}

		// Urgh, yeah this will eventually need to change
		if ( ImGui::BeginMenu( "Animations" ) ) {
			for ( unsigned int i = 0; static_cast<AnimationIndex>(i) < AnimationIndex::MAX_ANIMATIONS; ++i ) {
				const char *animationName = Model_GetAnimationDescription( i );
				if ( animationName == nullptr ) {
					continue;
				}

				bool selected = false;
				if ( ImGui::Selectable( animationName, selected ) ) {
					// todo
				}
			}
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "View" ) ) {
			if ( ImGui::MenuItem( "Fullscreen", nullptr, IsFullscreen() ) ) {
				ToggleFullscreen();
			}
			ImGui::Separator();
			if ( ImGui::MenuItem( "Rotate Model", nullptr, viewRotate ) ) {
				viewRotate = !viewRotate;
			}
			if ( ImGui::MenuItem( "Debug Normals", nullptr, viewDebugNormals ) ) {
				viewDebugNormals = !viewDebugNormals;
			}
			if ( ImGui::MenuItem( "Show Grid", nullptr, viewGrid ) ) {
				viewGrid = !viewGrid;
			}

#if 0 // Once we have reference counting for resources, add this back
			if ( modelPtr != nullptr ) {
				ImGui::Separator();

				if ( ImGui::BeginMenu( "Textures" ) ) {
					for ( unsigned int i = 0; i < modelPtr->levels[ 0 ].num_meshes; ++i ) {
						PLMesh *mesh = modelPtr->levels[ 0 ].meshes[ i ];
						if ( ImGui::ImageButton( reinterpret_cast<ImTextureID>( mesh->texture->internal.id ),
												 ImVec2( 128, 128 ) ) ) {
							ImGuiImpl_RegisterWindow( new TextureViewer( mesh->texture ) );
						}
					}

					ImGui::EndMenu();
				}
			}
#endif

			ImGui::Separator();

			ImGui::Text( "Camera Properties" );
			float fov = camera->GetFieldOfView();
			if ( ImGui::SliderFloat( "Fov", &fov, 0.0f, 180.0f, "%.f" ) ) {
				camera->SetFieldOfView( fov );
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		const char *labelStr = "No model loaded";
		if ( modelPtr != nullptr ) {
			labelStr = modelPtr->path;
		}
		ImGui::Text( "%s", labelStr );

		ImGui::Separator();

		unsigned int numTriangles = 0;
		unsigned int numVertices = 0;
		unsigned int numDrawCalls = 0;
		if ( modelPtr != nullptr ) {
			numDrawCalls = modelPtr->levels[ 0 ].num_meshes;
			for ( unsigned int i = 0; i < modelPtr->levels[ 0 ].num_meshes; ++i ) {
				numTriangles += modelPtr->levels[ 0 ].meshes[ i ]->num_triangles;
				numVertices += modelPtr->levels[ 0 ].meshes[ i ]->num_verts;
			}
		}
		ImGui::Text( "Draw Calls: %d", numDrawCalls );
		ImGui::Text( "Vertices: %d", numVertices );
		ImGui::Text( "Triangles: %d", numTriangles );

		ImGui::EndMenuBar();
	}

#if 0
	if ( ImGui::ListBoxHeader( "Models", modelList.size() ) ) {

		ImGui::ListBoxFooter();
	}

	ImGui::SameLine();
#endif

	ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0, 0, 0, 0 ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0, 0, 0, 0 ) );

	ImVec2 region = ImGui::GetContentRegionAvail();
	GenerateFrameBuffer( static_cast< unsigned int>( region.x ), static_cast< unsigned int>( region.y ) );
	ImGui::ImageButton(
		reinterpret_cast<ImTextureID>(textureAttachment->internal.id),
		region, ImVec2( 0, 0 ), ImVec2( 1, 1 ), 0 );
	if ( ImGui::IsItemHovered() ) {
		float newXPos = ImGui::GetMousePos().x - oldMousePos[ 0 ];
		float newYPos = ImGui::GetMousePos().y - oldMousePos[ 1 ];
		const static float posMod = 200.0f;
		if ( ImGui::IsMouseDown( ImGuiMouseButton_Left ) ) { // Rotation
			modelRotation.x += newYPos / posMod * g_state.last_draw_ms;
			modelRotation.y += newXPos / posMod * g_state.last_draw_ms;
		} else if ( ImGui::IsMouseDown( ImGuiMouseButton_Middle ) ) { // Panning
			PLVector3 position = camera->GetPosition();
			position.z += newXPos / posMod * g_state.last_draw_ms;
			position.y += newYPos / posMod * g_state.last_draw_ms;
			camera->SetPosition( position );
		} else {
			oldMousePos[ 0 ] = ImGui::GetMousePos().x;
			oldMousePos[ 1 ] = ImGui::GetMousePos().y;
		}

		// Handle mouse wheel movements
		PLVector3 position = camera->GetPosition();
		position.x += ImGui::GetIO().MouseWheel * 10;
		camera->SetPosition( position );
	}

	ImGui::PopStyleColor( 3 );

	ImGui::End();
}

void ModelViewer::AppendModelList( const char *path ) {
	modelList.push_back( path );
}

void ModelViewer::GenerateFrameBuffer( unsigned int width, unsigned int height ) {
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
