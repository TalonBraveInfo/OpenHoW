// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include <imgui.h>

#include "App.h"
#include "BaseWindow.h"

void BaseWindow::Begin( const std::string &windowTitle, unsigned int flags ) {
	if ( IsFullscreen() ) {
		flags |= ImGuiWindowFlags_NoDecoration;

		ohw::Display *display = ohw::GetApp()->GetDisplay();
		if ( display == nullptr ) {
			Error( "Attempted to setup ImGui window before display initialized!\n" );
		}

		int width, height;
		display->GetDisplaySize( &width, &height );

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
	curWindowSize = hei::Vector2( size.x, size.y );

	ImVec2 position = ImGui::GetWindowPos();
	curWindowPosition = hei::Vector2( position.x, position.y );
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
