// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include "BaseWindow.h"

class TerrainImportWindow : public BaseWindow {
public:
	TerrainImportWindow();
	~TerrainImportWindow() override;

	void Display() override;

protected:
private:
	void ImportTerrain();

	int multiplier_{ 128 };

	char path_buffer[32]{ '\0' };
};