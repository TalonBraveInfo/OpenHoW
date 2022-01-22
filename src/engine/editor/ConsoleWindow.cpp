// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include <imgui.h>

#include "App.h"
#include "ConsoleWindow.h"

ohw::ConsoleWindow::ConsoleWindow() : BaseWindow() {

}

ohw::ConsoleWindow::~ConsoleWindow() {
	Clear();
}

void ohw::ConsoleWindow::Display() {
	if ( !GetStatus() ) {
		return;
	}

	unsigned int flags =
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize( ImVec2( 512, 256 ), ImGuiCond_Once );

	Begin( "Console", flags );

	if ( ImGui::BeginMenuBar() ) {
		ImGui::EndMenuBar();
	}

	if ( ImGui::InputText( "Input", inputBuffer, sizeof( inputBuffer ), ImGuiInputTextFlags_EnterReturnsTrue ) && inputBuffer[ 0 ] != '\0' ) {
		PushCommand();
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Submit" ) ) {
		PushCommand();
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Clear" ) ) {
		Clear();
	}

	ImGui::Separator();

	const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild( "ScrollingRegion", ImVec2( 0, -footerHeightToReserve ), false, ImGuiWindowFlags_HorizontalScrollbar );

	for ( const auto &i : logBuffer ) {
		// todo: just respect the original message colour instead...
		if ( i.level == ohw::LOG_LEVEL_WARNING ) {
			ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.8f, 0.6f, 0.0f, 1.0f ) );
		} else if ( i.level == ohw::LOG_LEVEL_ERROR ) {
			ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.8f, 0.2f, 0.0f, 1.0f ) );
		} else {
			ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.0f, 0.6f, 0.8f, 1.0f ) );
		}

		ImGui::TextUnformatted( i.buffer );

		ImGui::PopStyleColor();
	}

	if ( scrollToEnd ) {
		ImGui::SetScrollHereY( 1.0f );
		scrollToEnd = false;
	}

	ImGui::EndChild();

	ImGui::End();
}

void ohw::ConsoleWindow::PushMessage( int level, const char *msg ) {
	if ( logBuffer.size() > 1024 ) {
		logBuffer.erase( logBuffer.begin() );
	}

	Message message;
	message.level = level;
	snprintf( message.buffer, sizeof( message.buffer ), "%s", msg );
	logBuffer.push_back( message );

	scrollToEnd = true;
}

void ohw::ConsoleWindow::Clear() {
	logBuffer.clear();
}

void ohw::ConsoleWindow::PushCommand() {
	PlParseConsoleString( inputBuffer );
	inputBuffer[ 0 ] = '\0';
}
