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

#include "window_model_viewer.h"

std::list<std::string> ModelViewer::modelList;

ModelViewer::ModelViewer() {
	plScanDirectory( "chars", "vtx", &ModelViewer::AppendModelList, true );

	modelList.sort();
	modelList.unique();
}

ModelViewer::~ModelViewer() {
	plDestroyModel( modelPtr );
}

void ModelViewer::Display() {
	ImGui::SetNextWindowSize( ImVec2( 512, 512 ), ImGuiCond_Once );
	ImGui::Begin( dname( "Model Viewer" ), &status_,
				  ImGuiWindowFlags_MenuBar |
					  ImGuiWindowFlags_HorizontalScrollbar |
					  ImGuiWindowFlags_NoSavedSettings );

	if ( ImGui::BeginMenuBar() ) {
		if ( ImGui::BeginMenu( "View" ) ) {
			if ( ImGui::MenuItem( "Rotate Model", nullptr, viewRotate ) ) {
				viewRotate = !viewRotate;
			}
			if ( ImGui::MenuItem( "Debug Normals", nullptr, viewDebugNormals ) ) {
				viewDebugNormals = !viewDebugNormals;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if ( ImGui::ListBoxHeader( "Models", modelList.size() ) ) {
		for ( const auto& i : modelList ) {
			if ( ImGui::Selectable( i.c_str() ) ) {
				plDestroyModel( modelPtr );
				modelPtr = nullptr;

				modelPtr = plLoadModel( i.c_str() );
				if ( modelPtr == nullptr ) {
					LogWarn( "Failed to load \"%s\" (%s)!\n", i.c_str(), plGetError() );
				}
			}
		}
		ImGui::ListBoxFooter();
	}

	float w = ImGui::GetWindowWidth() - 10;
	ImGui::Image( 0, ImVec2( w, w / 2 ) );

	//ImGui::Text( "Path: %s", modelPath.c_str() );
	ImGui::End();
}

void ModelViewer::AppendModelList( const char *path ) {
	modelList.push_back( path );
}
