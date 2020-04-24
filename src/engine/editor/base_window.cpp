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

#include "base_window.h"

void BaseWindow::Begin( const std::string &windowTitle, unsigned int flags ) {
	if ( IsFullscreen() ) {
		flags |= ImGuiWindowFlags_NoDecoration;

		int width, height;
		bool fullscreen;
		System_GetWindowDrawableSize( &width, &height, &fullscreen );

		ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
		ImGui::SetNextWindowSize( ImVec2( static_cast< float >( width ), static_cast< float >( height ) ),
								  ImGuiCond_Always );
	} else if ( oldSize != 0 || oldPosition != 0 ) {
		ImGui::SetNextWindowSize( ImVec2( oldSize.x, oldSize.y ), ImGuiCond_Always );
		ImGui::SetNextWindowPos( ImVec2( oldPosition.x, oldPosition.y ), ImGuiCond_Always );
		oldPosition = oldSize = 0;
	}

	ImGui::Begin( dname( windowTitle ), &status_, flags );

	ImVec2 size = ImGui::GetWindowSize();
	curWindowSize = PLVector2( size.x, size.y );

	ImVec2 position = ImGui::GetWindowPos();
	curWindowPosition = PLVector2( position.x, position.y );
}

void BaseWindow::ToggleFullscreen() {
	isFullscreen = !isFullscreen;
	if ( isFullscreen ) {
		oldSize.x = curWindowSize.x;
		oldSize.y = curWindowSize.y;
		oldPosition.x = curWindowPosition.x;
		oldPosition.y = curWindowPosition.y;
	}
}
