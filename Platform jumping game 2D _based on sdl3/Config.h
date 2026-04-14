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

	constexpr int ATTACK_WIDTH = 32;
	constexpr int ATTACK_HEIGHT = 32;

	constexpr float DELTAFREAM = 0.016f; // 0.016秒对应60帧每秒的更新频率
	constexpr int TARGET_RENDER_FPS = 120;

	constexpr float GRAVITY = 500.0f; // 重力加速度，单位像素/秒²
	constexpr float JUMP_VELOCITY = -320.0f; // 玩家跳跃初始速度，单位像素/秒
	constexpr int TILE_SIZE = 32; // 瓦片大小，单位像素
	constexpr float EPSILON = 0.001f; // 碰撞检测中的微小偏移量，避免浮点数精度问题

	constexpr float ATTACK_DURATION = 0.3f; // 攻击持续时间，单位秒
	constexpr float SPRINT_DURATION = 0.2f; // 冲刺持续时间，单位秒
	constexpr float DROP_DOWN_BUFFER_TIME = 0.5f; // 下降持续时间，单位秒

	constexpr float ACCELERATION = 1000.0f; // 玩家水平加速度，单位像素/秒² 
	constexpr float DECELERATION = 1500.0f; // 玩家水平减速度，单位像素/秒² 
	constexpr float AIR_ACCEL = 500.0f; // 玩家在空中时的水平加速度，单位像素/秒²
	constexpr float MAX_SPEED = 275.0f; // 玩家最大水平速度，单位像素/秒
	constexpr float SPRINT_MAX_SPEED = MAX_SPEED * 2.0f; // 冲刺时的最大水平速度，单位像素/秒
	constexpr float GROUND_FRICTION = 1500.0f; // 玩家在地面上的摩擦力，单位像素/秒²

	constexpr float JUMP_BUFFER_TIME = 0.2f; // 跳跃缓冲时间，单位秒
	constexpr float COYOTE_TIME = 0.1f; // coyote time（土狼时间），单位秒

	inline static const std::array<std::string, 3> LEVEL_PATH = {
		"resource/level1.bin",
		"resource/level2.bin",
		"resource/level3.bin"
	};

	constexpr int TILE_SRC_WIDTH = 16;
	constexpr int TILE_SRC_HEIGHT = 16;

	constexpr int PLAYER_1_Y_OFFSET = 0;
	constexpr int PLAYER_2_Y_OFFSET = 32;
	constexpr int PLAYER_3_Y_OFFSET = 32 * 2;

	inline static const std::array<int, 4> PLAYER_IDLE_SRC = { 0,0, 32 - 1, 32 - 1 }; // 玩家静止状态下的动画帧在纹理图中的位置和大小
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
	inline static const std::array<std::array<int, 4>, 4> PLAYER_ATTACK_SRC = { {
		{ 32 * 11, 0, 32 - 1, 32 - 1 }, // 玩家攻击动画帧1在纹理图中的位置和大小
		{ 32 * 10, 0, 32 - 1, 32 - 1 }, // 玩家攻击动画帧2在纹理图中的位置和大小
		{ 32 * 11, 0, 32 - 1, 32 - 1 }, // 玩家攻击动画帧3在纹理图中的位置和大小
		{ 32 * 12, 0, 32 - 1, 32 - 1 }, // 玩家攻击动画帧4在纹理图中的位置和大小
	} };
	inline static const std::array<std::array<int, 4>, 3> PLAYER_SPRINT_SRC = { {
		{ 32 * 11, 0, 32 - 1, 32 - 1 }, // 玩家冲刺动画帧1在纹理图中的位置和大小
		{ 32 * 13, 0, 32 - 1, 32 - 1 }, // 玩家冲刺动画帧2在纹理图中的位置和大小
		{ 32 * 11, 0, 32 - 1, 32 - 1 }, // 玩家冲刺动画帧3在纹理图中的位置和大小
	} };
	inline static const std::array<std::array<int, 4>, 4> ATTACK_SRC = { {
		{ 0, 0, 32, 32 }, // 攻击效果动画帧1在纹理图中的位置和大小
		{ 32, 0, 32 , 32}, // 攻击效果动画帧2在纹理图中的位置和大小
		{ 64, 0, 32 , 32 }, // 攻击效果动画帧3在纹理图中的位置和大小
		{ 96, 0, 32 , 32}, // 攻击效果动画帧4在纹理图中的位置和大小
	} };

	constexpr int SPRINT_MAX_COUNT = 1; // 玩家最大连续冲刺次数，后续可以根据实际设计调整

	constexpr int TMX_LAYER_NUMS = 3; // tmx地图层数，后续增加地图资源管理等功能时可以根据实际地图层数调整
	//constexpr int 
}


