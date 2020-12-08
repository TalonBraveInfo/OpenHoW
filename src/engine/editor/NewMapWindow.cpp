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

#include "App.h"
#include "NewMapWindow.h"

using namespace ohw;

void NewMapWindow::Display() {
	ImGui::SetNextWindowSize( ImVec2( 256, 128 ), ImGuiCond_Once );
	ImGui::Begin( "New Map", &status_, ED_DEFAULT_WINDOW_FLAGS );
	ImGui::InputText( "Name", name_buffer_, sizeof( name_buffer_ ) );
	//ImGui::InputText("Author", author_buffer_, sizeof(author_buffer_));
	ImGui::Separator();
	if ( ImGui::Button( "Create" ) ) {
		GetApp()->gameManager->CreateManifest( name_buffer_ );
		GetApp()->gameManager->LoadMap( name_buffer_ );
		SetWindowStatus( false );
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Cancel" ) ) {
		SetWindowStatus( false );
	}
	ImGui::End();
}
