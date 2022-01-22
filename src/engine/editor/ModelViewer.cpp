// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "graphics/ShaderManager.h"
#include "graphics/Camera.h"
#include "imgui_layer.h"
#include "model.h"
#include "Language.h"

#include "ModelViewer.h"
#include "TextureViewer.h"

#define VIEWER_WIDTH  640
#define VIEWER_HEIGHT 480

std::list< std::string > ohw::ModelViewer::modelList;

static const char *supportedModelFormats[] = { "obj", "vtx", "min", nullptr };

ohw::ModelViewer::ModelViewer() : BaseWindow() {
	const char **formatExtensions = supportedModelFormats;
	while ( *formatExtensions != nullptr ) {
		PlScanDirectory( "chars", *formatExtensions, &ModelViewer::AppendModelList, true, nullptr );
		formatExtensions++;
	}

	modelList.sort();
	modelList.unique();

	camera = new Camera( { -2500.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );
	camera->SetViewport( 0, 0, 640, 480 );

	GenerateFrameBuffer( 640, 480 );
}

ohw::ModelViewer::~ModelViewer() {
	PlgDestroyTexture( textureAttachment );
	PlgDestroyFrameBuffer( drawBuffer );
}

void ohw::ModelViewer::DrawViewport() {
	PlgBindFrameBuffer( drawBuffer, PLG_FRAMEBUFFER_DRAW );

	PlgSetDepthBufferMode( PLG_DEPTHBUFFER_ENABLE );

	PlgSetClearColour( { 0, 0, 0, 0 } );
	PlgClearBuffers( PLG_BUFFER_COLOUR | PLG_BUFFER_DEPTH );

	unsigned int bufferWidth = 0, bufferHeight = 0;
	PlgGetFrameBufferResolution( drawBuffer, &bufferWidth, &bufferHeight );
	camera->SetViewport( 0, 0, bufferWidth, bufferHeight );
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

	if ( model == nullptr ) {
		PlgBindFrameBuffer( nullptr, PLG_FRAMEBUFFER_DRAW );
		return;
	}

	ShaderProgram *shaderProgram = Shaders_GetProgram( viewDebugNormals ? "debug_normals" : "generic_textured_lit" );
	shaderProgram->Enable();

	if ( !viewDebugNormals ) {
		hei::Vector3 sunPosition( 0.5f, 0.2f, 0.6f );
		PLVector4 sunColour = { 1.0f, 1.0f, 1.0f, 1.0f };
		PLVector4 ambience = { 0.75f, 0.75f, 0.75f, 1.0f };

		PlgSetShaderUniformValue( shaderProgram->GetInternalProgram(), "sun_position", &sunPosition, false );
		PlgSetShaderUniformValue( shaderProgram->GetInternalProgram(), "sun_colour", &sunColour, false );
		PlgSetShaderUniformValue( shaderProgram->GetInternalProgram(), "ambient_colour", &ambience, false );
	}
	
	if ( viewRotate ) {
		modelRotation.y += 0.01f * GetApp()->GetDeltaTime();
	}

	hei::Matrix4 mat;
	mat.Identity();
	mat.Rotate( angles.z, { 1, 0, 0 } );
	mat.Rotate( angles.y, { 0, 1, 0 } );
	mat.Rotate( angles.x, { 0, 0, 1 } );

	model->modelMatrix = mat;
	model->Draw( false );

	if ( viewDebugNormals ) {
		model->DrawNormals();
	}

	if ( viewSkeleton ) {
		model->DrawSkeleton();
	}

	PlgBindFrameBuffer( nullptr, PLG_FRAMEBUFFER_DRAW );
}

void ohw::ModelViewer::Display() {
	// Draw the model view if there's a valid model
	DrawViewport();

	unsigned int flags =
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize( ImVec2( VIEWER_WIDTH, VIEWER_HEIGHT ), ImGuiCond_Once );

	Begin( lm_gtr( "$window_model_viewer" ), flags );

	if ( ImGui::BeginMenuBar() ) {
		if ( ImGui::BeginMenu( "Models" ) ) {
			static ImGuiTextFilter filter;
			filter.Draw();

			for ( const auto &i : modelList ) {
				if ( !filter.PassFilter( i.c_str() ) ) {
					continue;
				}

				bool selected = false;
				if ( model != nullptr ) {
					selected = ( i == model->GetPath() );
				}

				if ( ImGui::Selectable( i.c_str(), selected ) ) {
					if ( model != nullptr ) {
						model->Release();
						model = nullptr;

						// Force a cleanup
						GetApp()->resourceManager->ClearAllResources();
					}

					model = GetApp()->resourceManager->LoadModel( i );
				}
			}
			ImGui::EndMenu();
		}

		// Urgh, yeah this will eventually need to change
		if ( model != nullptr && model->IsAnimated() ) {
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
		}

		if ( ImGui::BeginMenu( "View" ) ) {
			if ( ImGui::MenuItem( "Fullscreen", nullptr, IsFullscreen() ) ) {
				ToggleFullscreen();
			}
			ImGui::Separator();
			if ( ImGui::MenuItem( "Rotate Model", nullptr, viewRotate ) ) {
				viewRotate = !viewRotate;
			}
			ImGui::Separator();
			if ( ImGui::MenuItem( "Show Normals", nullptr, viewDebugNormals ) ) {
				viewDebugNormals = !viewDebugNormals;
			}
			if ( model != nullptr && model->IsAnimated() ) {
				if ( ImGui::MenuItem( "Show Skeleton", nullptr, viewSkeleton ) ) {
					viewSkeleton = !viewSkeleton;
				}
			}
			if ( ImGui::MenuItem( "Show Grid", nullptr, viewGrid ) ) {
				viewGrid = !viewGrid;
			}

			if ( model != nullptr ) {
				ImGui::Separator();

				if ( ImGui::BeginMenu( "Textures" ) ) {
					for ( unsigned int i = 0; i < model->GetNumberOfMeshes(); ++i ) {
						PLGMesh *mesh = model->GetInternalMesh( i );
						if ( ImGui::ImageButton( reinterpret_cast<ImTextureID>( mesh->texture->internal.id ), ImVec2( 128, 128 ) ) ) {
							//ImGuiImpl_RegisterWindow( new TextureViewer( model->GetTextureResource( i ) ) );
						}
					}

					ImGui::EndMenu();
				}
			}

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
		if ( model != nullptr ) {
			labelStr = model->GetPath().c_str();
		}
		ImGui::Text( "%s", labelStr );

		ImGui::Separator();

		unsigned int numTriangles = 0;
		unsigned int numVertices = 0;
		unsigned int numDrawCalls = 0;
		if ( model != nullptr ) {
			numDrawCalls = model->GetNumberOfMeshes();
			for ( unsigned int i = 0; i < numDrawCalls; ++i ) {
				PLGMesh *mesh = model->GetInternalMesh( i );
				numTriangles += mesh->num_triangles;
				numVertices += mesh->num_verts;
			}
		}
		ImGui::Text( "Batches: %d", numDrawCalls );
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
			modelRotation.x += newYPos / posMod * GetApp()->GetDeltaTime();
			modelRotation.y += newXPos / posMod * GetApp()->GetDeltaTime();
		} else if ( ImGui::IsMouseDown( ImGuiMouseButton_Middle ) ) { // Panning
			PLVector3 position = camera->GetPosition();
			position.z += newXPos / posMod * GetApp()->GetDeltaTime();
			position.y += newYPos / posMod * GetApp()->GetDeltaTime();
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

void ohw::ModelViewer::AppendModelList( const char *path, void *userData ) {
	u_unused( userData );

	modelList.push_back( path );
}

void ohw::ModelViewer::GenerateFrameBuffer( unsigned int width, unsigned int height ) {
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
