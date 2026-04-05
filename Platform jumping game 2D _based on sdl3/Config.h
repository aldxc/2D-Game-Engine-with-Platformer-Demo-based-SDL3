#pragma once
#include <string>

namespace Config {
	// 窗口尺寸 tile 32×32 - 32×18瓦片
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

	constexpr int PLAYER_WIDTH = 32;
	constexpr int PLAYER_HEIGHT = 32;

	constexpr float DELTAFREAM = 0.016f; // 假设每帧16ms，约60FPS

	constexpr float GRAVITY = 500.0f; // 重力加速度，单位像素/秒²
	constexpr float PLAYER_SPEED = 200.0f; // 玩家水平移动速度，单位像素/秒
	constexpr float JUMP_VELOCITY = -300.0f; // 玩家跳跃初始速度，单位像素/秒
	constexpr float TILE_SIZE = 32.0f; // 瓦片大小，单位像素
	constexpr float EPSILON = 0.001f; // 碰撞检测中的微小偏移量，避免浮点数精度问题
}


