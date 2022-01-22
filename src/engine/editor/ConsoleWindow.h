// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include "BaseWindow.h"

namespace ohw {
	class ConsoleWindow : public BaseWindow {
	public:
		ConsoleWindow();
		~ConsoleWindow() override;

		void Display() override;

		void PushMessage( int level, const char *msg );
		void Clear();

		void PushCommand();

	private:
		struct Message {
			unsigned int level{ 0 };
			char buffer[ 512 ]{ '\0' };
		};
		std::vector< Message > logBuffer;

		bool scrollToEnd{ false };

		char inputBuffer[ 512 ]{ '\0' };
	};
}
