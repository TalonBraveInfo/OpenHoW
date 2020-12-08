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
#include "game/Player.h"
#include "NewGameWindow.h"

using namespace ohw;

void NewGameWindow::Display() {
	ImGui::SetNextWindowSize( ImVec2( 320, 400 ), ImGuiCond_Once );
	ImGui::Begin( "New Game", &status_, ED_DEFAULT_WINDOW_FLAGS );

	// Team Selection
	ImGui::Text( "Select Team" );
	GameManager::PlayerTeamVector teams = GetApp()->gameManager->GetDefaultTeams();
	std::vector< const char * > options;
	for ( const auto &team : teams ) {
		options.push_back( team.name.c_str() );
	}

	static int selectedTeamNum = 0;
	if ( ImGui::ListBox( "Teams", &selectedTeamNum, &options[ 0 ], options.size(), 10 ) ) {
		snprintf( teamName, sizeof( teamName ), "%s", teams[ selectedTeamNum ].name.c_str() );
		selectedTeam = teams[ selectedTeamNum ];
	}

	if ( ImGui::InputText( "Team Name", teamName, sizeof( teamName ) ) ) {
		selectedTeam.name = teamName;
	}

	if ( ImGui::Button( "Configure Team" ) ) {
		// todo
	}

	ImGui::Separator();

	ImGui::Checkbox( "Play training mission?", &playTrainingMission );

	if ( ImGui::Button( "Start Game" ) ) {
		// Setup local player
		Player *player = new Player( PlayerType::LOCAL );
		player->SetTeam( selectedTeam );

		PlayerPtrVector players = {
				player
		};

		GameModeDescriptor game_mode_descriptor;
		std::string mapname;
		if ( !playTrainingMission ) {
			mapname = "estu";

			// crap for now, need to fill this in with the correct data...
			game_mode_descriptor.default_health = 100;
			game_mode_descriptor.desired_mode = "sp";
			game_mode_descriptor.select_pig = false;
			game_mode_descriptor.turn_time = 60;
			game_mode_descriptor.sudden_death = false;

			Player *enemy = new Player( PlayerType::COMPUTER );
			unsigned int enemy_team = selectedTeamNum + 1;
			if ( enemy_team >= teams.size() ) {
				enemy->SetTeam( teams[ enemy_team - 2 ] );
			} else {
				enemy->SetTeam( teams[ enemy_team ] );
			}

			players.push_back( enemy );
		} else {
			mapname = "camp";
			// crap for now, need to fill this in with the correct data...
			game_mode_descriptor.default_health = 100;
			game_mode_descriptor.desired_mode = "sp";
			game_mode_descriptor.select_pig = false;
			game_mode_descriptor.turn_time = 60;
			game_mode_descriptor.sudden_death = false;
		}

		GetApp()->gameManager->StartMode( mapname, players, game_mode_descriptor );

		SetWindowStatus( false );
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Cancel" ) ) {
		SetWindowStatus( false );
	}
	ImGui::End();
}
