#pragma once
#include <string>
#include <array>
#include <cstdint>

namespace Config {
	// 窗口尺寸 tile 32×32 - 32×18瓦片
	constexpr int WINDOW_WIDTH = 1920;
	constexpr int WINDOW_HEIGHT = 1080;

	constexpr int LOGIC_WIDTH = 1024;
	constexpr int LOGIC_HEIGHT = 576;

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

	constexpr float DELTAFREAM = 0.016f; // 0.016秒对应60帧每秒的更新频率
	constexpr int TARGET_RENDER_FPS = 120;

	constexpr float GRAVITY = 500.0f; // 重力加速度，单位像素/秒²
	constexpr float PLAYER_SPEED = 200.0f; // 玩家水平移动速度，单位像素/秒
	constexpr float JUMP_VELOCITY = -320.0f; // 玩家跳跃初始速度，单位像素/秒
	constexpr float TILE_SIZE = 32.0f; // 瓦片大小，单位像素
	constexpr float EPSILON = 0.001f; // 碰撞检测中的微小偏移量，避免浮点数精度问题

	constexpr float ACCELERATION = 1000.0f; // 玩家水平加速度，单位像素/秒² 
	constexpr float DECELERATION = 1500.0f; // 玩家水平减速度，单位像素/秒² 
	constexpr float AIR_ACCEL = 500.0f; // 玩家在空中时的水平加速度，单位像素/秒²
	constexpr float MAX_SPEED = 275.0f; // 玩家最大水平速度，单位像素/秒
	constexpr float GROUND_FRICTION = 1500.0f; // 玩家在地面上的摩擦力，单位像素/秒²

	constexpr float JUMP_BUFFER_TIME = 0.2f; // 跳跃缓冲时间，单位秒
	constexpr float COYOTE_TIME = 0.1f; // coyote time（土狼时间），单位秒

	inline static const std::array<std::string, 3> LEVEL_PATH = {
		"resource/level1.bin",
		"resource/level2.bin",
		"resource/level3.bin"
	};

	//减一是为了边缘采样，避免渲染时出现边缘像素的混合问题
	inline static const std::array<std::array<int, 4>, 8> TILE_LAND_SRC = { {
		{ 160, 0, 16 - 1, 16 - 1 }, // 左侧陆地瓦片在瓦片图中的位置和大小
		{ 176, 0, 16 - 1, 16 - 1 }, // 陆地中间瓦片在瓦片图中的位置和大小
		{ 192, 0, 16 - 1, 16 - 1 }, // 右侧陆地瓦片在瓦片图中的位置和大小
		{ 160, 16, 16 - 1, 16 - 1},	// 地下左侧瓦片在瓦片图中的位置和大小
		{ 176, 16, 16 - 1, 16 - 1},	// 地下中间瓦片在瓦片图中的位置和大小
		{ 192, 16, 16 - 1, 16 - 1},	// 地下右侧瓦片在瓦片图中的位置和大小
		{ 208, 0, 16 - 1, 16 - 1 }, // 单独地面瓦片在瓦片图中的位置和大小
		{ 208, 16, 16 - 1, 16 - 1}	// 单独地下瓦片在瓦片图中的位置和大小
	} };

	inline static const std::array<std::array<int, 4>, 5> TILE_GRASSLAND_SRC = { {
		{ 112, 0, 16 - 1, 16 - 1 }, // 非陆地边缘草地左侧边缘
		{ 128, 0, 16 - 1, 16 - 1 }, // 草地中心
		{ 144, 0, 16 - 1, 16 - 1 }, // 非陆地边缘草地右侧边缘
		{ 128, 16, 16 - 1, 16 - 1},	// 左侧陆地边缘草地边缘
		{ 144, 16, 16 - 1, 16 - 1}	// 右侧陆地边缘草地边缘
	} };

	inline static const std::array<int, 4> BK_SRC = { 0, 0, 112 - 1, 128 - 1 }; // 背景瓦片在瓦片图中的位置和大小
	inline static const std::array<int, 4> PLAYER_IDLE_SRC = { 0,0, 32 - 1, 32 - 1 }; // 玩家静止状态下的动画帧在纹理图中的位置和大小

	constexpr int PLAYER_1_Y_OFFSET = 0;
	constexpr int PLAYER_2_Y_OFFSET = 32;
	constexpr int PLAYER_3_Y_OFFSET = 32 * 2;

	inline static const std::array<std::array<int, 4>, 4> PLAYER_RUN_SRC = { {
		{ 32, 0, 32 - 1, 32 - 1 }, // 玩家跑动动画帧1在纹理图中的位置和大小
		{ 64, 0, 32 - 1, 32 - 1 }, // 玩家跑动动画帧2在纹理图中的位置和大小
		{ 96, 0, 32 - 1, 32 - 1 }, // 玩家跑动动画帧3在纹理图中的位置和大小
		{ 0, 0, 32 - 1, 32 - 1 }, // 玩家跑动动画帧4在纹理图中的位置和大小
	} };
	inline static const std::array<std::array<int, 4>, 2> PLAYER_JUMP_SRC = { {
		{ 32 * 4, 0, 32 - 1, 32 - 1 }, // 玩家跳跃动画帧1在纹理图中的位置和大小 // 起跳帧
		{ 32 * 5, 0, 32 - 1, 32 - 1 }, // 玩家跳跃动画帧2在纹理图中的位置和大小	// 上升帧
	} };
	inline static const std::array<std::array<int, 4>, 2> PLAYER_FALL_SRC = { {
		{ 32 * 6, 0, 32 - 1, 32 - 1 }, // 玩家跳跃动画帧3在纹理图中的位置和大小 // 最高点帧
		{ 32 * 7, 0, 32 - 1, 32 - 1 }, // 玩家跳跃动画帧4在纹理图中的位置和大小 // 下降帧
	} };

}


