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
#include "Language.h"
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
		switch ( i.level ) {
			default:
				ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.0f, 0.6f, 0.8f, 1.0f ) );
				break;
			case LOG_LEVEL_WARNING:
				ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.8f, 0.6f, 0.0f, 1.0f ) );
				break;
			case LOG_LEVEL_ERROR:
				ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.8f, 0.2f, 0.0f, 1.0f ) );
				break;
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
	plParseConsoleString( inputBuffer );
	inputBuffer[ 0 ] = '\0';
}
