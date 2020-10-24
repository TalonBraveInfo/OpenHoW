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

#include <imgui.h>

#include "../engine.h"
#include "../Map.h"
#include "../Terrain.h"

#include "TerrainImportWindow.h"

using namespace ohw;

TerrainImportWindow::TerrainImportWindow() = default;
TerrainImportWindow::~TerrainImportWindow() = default;

void TerrainImportWindow::Display() {
	ImGui::SetNextWindowSize( ImVec2( 310, 128 ), ImGuiCond_Once );
	Begin( "Import Heightmap", ED_DEFAULT_WINDOW_FLAGS );
	ImGui::InputText( "Path", path_buffer, sizeof( path_buffer ) );
	ImGui::InputInt( "Multiplier", &multiplier_ );
	if ( ImGui::Button( "Import" ) ) {
		ImportTerrain();
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Cancel" ) ) {
		SetStatus( false );
	}
	ImGui::End();
}

void TerrainImportWindow::ImportTerrain() {
	Map *map = Engine::Game()->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	Terrain *terrain = map->GetTerrain();
	if ( terrain == nullptr ) {
		return;
	}

	terrain->LoadHeightmap( path_buffer, multiplier_ );
}
