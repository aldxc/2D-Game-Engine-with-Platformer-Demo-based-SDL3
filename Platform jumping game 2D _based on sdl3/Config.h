#pragma once
#include <string>

namespace Config {
	// ¥∞ø⁄≥ﬂ¥Á tile 32°¡32 - 32°¡18Õﬂ∆¨
	constexpr int WINDOW_WIDTH = 1024;
	constexpr int WINDOW_HEIGHT = 576;

	constexpr int MENU_BOTTOMS_NUM = 4;
	constexpr int WON_BUTTONS_NUM = 3;
	constexpr int LOSE_BUTTONS_NUM = 2;

	inline static const std::string PATH_FONT = "font.ttf";
	constexpr int DEFAULT_TEXT_SIZE = 30;

	constexpr int MENU_BUTTON_WIDTH = 200;
	constexpr int MENU_BUTTON_HEIGHT = 50;
	constexpr int WON_BUTTON_WIDTH = 150;
	constexpr int WON_BUTTON_HEIGHT = 40;
	constexpr int LOSE_BUTTON_WIDTH = 150;
	constexpr int LOSE_BUTTON_HEIGHT = 40;

	constexpr int PLAYER_WIDTH = 50;
	constexpr int PLAYER_HEIGHT = 50;

	constexpr float DELTAFREAM = 0.016f; // ºŸ…Ë√ø÷°16ms£¨‘º60FPS

}


