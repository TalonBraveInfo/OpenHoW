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
#include "../Language.h"
#include "../game/player.h"

#include "window_new_game.h"

using namespace ohw;

void NewGameWindow::Display() {
	ImGui::SetNextWindowSize( ImVec2( 320, 400 ), ImGuiCond_Once );
	ImGui::Begin( "New Game", &status_, ED_DEFAULT_WINDOW_FLAGS );

	// Team Selection
	ImGui::Text( "Select Team" );
	GameManager::PlayerTeamVector teams = Engine::Game()->GetDefaultTeams();
	std::vector<const char *> options;
	for ( const auto &team : teams ) {
		options.push_back( team.name.c_str() );
	}

	static int selected_team = 0;
	if ( ImGui::ListBox( "Teams", &selected_team, &options[ 0 ], options.size(), 10 ) ) {
		snprintf( team_name_, sizeof( team_name_ ), "%s", teams[ selected_team ].name.c_str() );
		team_ = teams[ selected_team ];
	}

	if ( ImGui::InputText( "Team Name", team_name_, sizeof( team_name_ ) ) ) {
		team_.name = team_name_;
	}

	if ( ImGui::Button( "Configure Team" ) ) {
		// todo
	}

	ImGui::Separator();

	ImGui::Checkbox( "Play training mission?", &training_mission_ );

	if ( ImGui::Button( "Start Game" ) ) {
		// Setup local player
		Player *player = new Player( PlayerType::LOCAL );
		player->SetTeam( team_ );

		PlayerPtrVector players = {
			player
		};

		GameModeDescriptor game_mode_descriptor;
		std::string mapname;
		if ( !training_mission_ ) {
			mapname = "estu";

			// crap for now, need to fill this in with the correct data...
			game_mode_descriptor.default_health = 100;
			game_mode_descriptor.desired_mode = "sp";
			game_mode_descriptor.select_pig = false;
			game_mode_descriptor.turn_time = 60;
			game_mode_descriptor.sudden_death = false;

			Player *enemy = new Player( PlayerType::COMPUTER );
			unsigned int enemy_team = selected_team + 1;
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

		Engine::Game()->StartMode( mapname, players, game_mode_descriptor );

		SetStatus( false );
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Cancel" ) ) {
		SetStatus( false );
	}
	ImGui::End();
}
