// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include <imgui.h>
#include <fstream>

#include "App.h"
#include "Map.h"
#include "MapConfigEditor.h"

ohw::MapConfigEditor::MapConfigEditor() {
	map_ = GetApp()->gameManager->GetCurrentMap();
	if ( map_ == nullptr ) {
		throw std::runtime_error( "Attempted to create config editor without a valid map loaded!\n" );
	}

	manifest_ = map_->GetManifest();
	backup_ = *manifest_;

	strncpy( name_buffer, manifest_->name.c_str(), sizeof( name_buffer ) );
	strncpy( author_buffer, manifest_->author.c_str(), sizeof( author_buffer ) );
}

ohw::MapConfigEditor::~MapConfigEditor() {
	RestoreManifest();
}

void ohw::MapConfigEditor::DisplayTemperature() {
	enum {
		TEMP_NORMAL, TEMP_HOT, TEMP_COLD, MAX_TEMP
	};
	static const char *temperatures[MAX_TEMP] = { "Normal", "Hot", "Cold" };
	static int temperature_index = -1;
	if ( temperature_index == -1 ) {
		if ( pl_strcasecmp( manifest_->temperature.c_str(), "hot" ) == 0 ) {
			temperature_index = TEMP_HOT;
		} else if ( pl_strcasecmp( manifest_->temperature.c_str(), "cold" ) == 0 ) {
			temperature_index = TEMP_COLD;
		} else {
			temperature_index = TEMP_NORMAL;
		}
	}

	if ( ImGui::BeginCombo( "Temperature", temperatures[ temperature_index ] ) ) {
		for ( int i = 0; i < MAX_TEMP; ++i ) {
			if ( ImGui::Selectable( temperatures[ i ], ( temperature_index == i ) ) ) {
				ImGui::SetItemDefaultFocus();
				temperature_index = i;
				manifest_->temperature = u_stringtolower( temperatures[ temperature_index ] );
			}
		}

		ImGui::EndCombo();
	}
}

void ohw::MapConfigEditor::DisplayWeather() {
	enum {
		WEATHER_CLEAR, WEATHER_RAIN, WEATHER_SNOW, MAX_WEATHER
	};
	static const char *weather_labels[MAX_WEATHER] = { "Clear", "Rain", "Snow" };
	static int weather_index = -1;
	if ( weather_index == -1 ) {
		if ( pl_strcasecmp( manifest_->weather.c_str(), "rain" ) == 0 ) {
			weather_index = WEATHER_RAIN;
		} else if ( pl_strcasecmp( manifest_->weather.c_str(), "snow" ) == 0 ) {
			weather_index = WEATHER_SNOW;
		} else {
			weather_index = WEATHER_CLEAR;
		}
	}

	if ( ImGui::BeginCombo( "Weather", weather_labels[ weather_index ] ) ) {
		for ( int i = 0; i < MAX_WEATHER; ++i ) {
			if ( ImGui::Selectable( weather_labels[ i ], ( weather_index == i ) ) ) {
				ImGui::SetItemDefaultFocus();
				weather_index = i;
				manifest_->weather = u_stringtolower( weather_labels[ weather_index ] );
			}
		}

		ImGui::EndCombo();
	}
}

void ohw::MapConfigEditor::DisplayTime() {
	enum {
		TIME_DAY, TIME_NIGHT, MAX_TIME
	};
	static const char *times[MAX_TIME] = { "Day", "Night" };
	static int time_index = -1;
	if ( time_index == -1 ) {
		if ( pl_strcasecmp( manifest_->time.c_str(), "day" ) == 0 ) {
			time_index = TIME_DAY;
		} else {
			time_index = TIME_NIGHT;
		}
	}

	if ( ImGui::BeginCombo( "Time", times[ time_index ] ) ) {
		for ( int i = 0; i < MAX_TIME; ++i ) {
			if ( ImGui::Selectable( times[ i ], ( time_index == i ) ) ) {
				ImGui::SetItemDefaultFocus();
				time_index = i;
				manifest_->time = u_stringtolower( times[ time_index ] );
			}
		}

		ImGui::EndCombo();
	}
}

void ohw::MapConfigEditor::Display() {
	ImGui::SetNextWindowSize( ImVec2( 310, 512 ), ImGuiCond_Once );
	Begin( "Map Config Editor", ED_DEFAULT_WINDOW_FLAGS );

	ImGui::InputText( "Name", name_buffer, sizeof( name_buffer ) );
	ImGui::InputText( "Author", author_buffer, sizeof( author_buffer ) );

	// todo: mode selection - need to query wherever this ends up being implemented...

	ImGui::Separator();

	DisplayTemperature();
	DisplayWeather();
	DisplayTime();

	ImGui::Separator();

	ImGui::Text( "Sky Settings" );

	float rgb[3];
	rgb[ 0 ] = PlByteToFloat( manifest_->sky_colour_top.r );
	rgb[ 1 ] = PlByteToFloat( manifest_->sky_colour_top.g );
	rgb[ 2 ] = PlByteToFloat( manifest_->sky_colour_top.b );
	if ( ImGui::ColorEdit3( "Top Colour", rgb, ImGuiColorEditFlags_InputRGB ) ) {
		manifest_->sky_colour_top.r = PlFloatToByte( rgb[ 0 ] );
		manifest_->sky_colour_top.g = PlFloatToByte( rgb[ 1 ] );
		manifest_->sky_colour_top.b = PlFloatToByte( rgb[ 2 ] );
		map_->UpdateSky();
	}

	rgb[ 0 ] = PlByteToFloat( manifest_->sky_colour_bottom.r );
	rgb[ 1 ] = PlByteToFloat( manifest_->sky_colour_bottom.g );
	rgb[ 2 ] = PlByteToFloat( manifest_->sky_colour_bottom.b );
	if ( ImGui::ColorEdit3( "Bottom Colour", rgb, ImGuiColorEditFlags_InputRGB ) ) {
		manifest_->sky_colour_bottom.r = PlFloatToByte( rgb[ 0 ] );
		manifest_->sky_colour_bottom.g = PlFloatToByte( rgb[ 1 ] );
		manifest_->sky_colour_bottom.b = PlFloatToByte( rgb[ 2 ] );
		map_->UpdateSky();
	}

	ImGui::Separator();

	ImGui::Text( "Lighting Settings" );

	ImGui::SliderAngle( "Sun Pitch", &manifest_->sun_pitch, -90, 90, nullptr );
	ImGui::SliderAngle( "Sun Yaw", &manifest_->sun_yaw, -180, 180, nullptr );

	rgb[ 0 ] = PlByteToFloat( manifest_->sun_colour.r );
	rgb[ 1 ] = PlByteToFloat( manifest_->sun_colour.g );
	rgb[ 2 ] = PlByteToFloat( manifest_->sun_colour.b );
	if ( ImGui::ColorEdit3( "Sun Colour", rgb, ImGuiColorEditFlags_InputRGB ) ) {
		manifest_->sun_colour.r = PlFloatToByte( rgb[ 0 ] );
		manifest_->sun_colour.g = PlFloatToByte( rgb[ 1 ] );
		manifest_->sun_colour.b = PlFloatToByte( rgb[ 2 ] );
	}

	rgb[ 0 ] = PlByteToFloat( manifest_->ambient_colour.r );
	rgb[ 1 ] = PlByteToFloat( manifest_->ambient_colour.g );
	rgb[ 2 ] = PlByteToFloat( manifest_->ambient_colour.b );
	if ( ImGui::ColorEdit3( "Ambient Colour", rgb, ImGuiColorEditFlags_InputRGB ) ) {
		manifest_->ambient_colour.r = PlFloatToByte( rgb[ 0 ] );
		manifest_->ambient_colour.g = PlFloatToByte( rgb[ 1 ] );
		manifest_->ambient_colour.b = PlFloatToByte( rgb[ 2 ] );
	}

	ImGui::Separator();

	// Fog
	{
		ImGui::Text( "Fog Settings" );

		rgb[ 0 ] = PlByteToFloat( manifest_->fog_colour.r );
		rgb[ 1 ] = PlByteToFloat( manifest_->fog_colour.g );
		rgb[ 2 ] = PlByteToFloat( manifest_->fog_colour.b );
		if ( ImGui::ColorEdit3( "Fog Colour", rgb, ImGuiColorEditFlags_InputRGB ) ) {
			manifest_->fog_colour.r = PlFloatToByte( rgb[ 0 ] );
			manifest_->fog_colour.g = PlFloatToByte( rgb[ 1 ] );
			manifest_->fog_colour.b = PlFloatToByte( rgb[ 2 ] );
		}

		ImGui::SliderFloat( "Fog Intensity", &manifest_->fog_intensity, -100.0f, 100.0f, "%.0f" );
		ImGui::SliderFloat( "Fog Distance", &manifest_->fog_distance, 0, 300.0f, "%.0f" );
	}

	ImGui::Separator();

	if ( ImGui::Button( "Save" ) ) {
		SaveManifest( "maps/" + manifest_->filename + ".map" );
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Cancel" ) ) {
		RestoreManifest();

		// Close the window
		SetWindowStatus( false );
	}

	ImGui::End();
}

void ohw::MapConfigEditor::SaveManifest( const std::string &path ) {
	const ModManager::ModDescription *currentMod = GetApp()->modManager->GetCurrentModDescription();
	std::ofstream output( "mods/" + currentMod->directory + path );
	if ( !output.is_open() ) {
		Warning( "Failed to write to \"%s\", aborting!n\"\n", filename_buffer );
		return;
	}

	manifest_->name = name_buffer;
	manifest_->author = author_buffer;

	output << manifest_->Serialize();

	Print( "Wrote \"%s\"!\n", path.c_str() );
	backup_ = *manifest_;
}

void ohw::MapConfigEditor::RestoreManifest() {
	*manifest_ = backup_;

	map_->UpdateSky();
}
