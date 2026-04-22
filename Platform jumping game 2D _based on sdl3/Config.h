#pragma once
#include <string>
#include <array>
#include <cstdint>
#include <cstddef>
#include "core/Rect.h"

namespace Config {
	// 窗口尺寸 tile 32×32 - 32×18瓦片
	constexpr int32_t WINDOW_WIDTH = 1920;
	constexpr int32_t WINDOW_HEIGHT = 1080;

	constexpr int32_t LOGIC_WIDTH = 1024;
	constexpr int32_t LOGIC_HEIGHT = 576;

	constexpr size_t MENU_BOTTOMS_NUM = 4;
	constexpr size_t WON_BUTTONS_NUM = 3;
	constexpr size_t LOSE_BUTTONS_NUM = 3;
	constexpr size_t PLAYING_BUTTONS_NUM = 4; // 测试按钮
	constexpr size_t SELECT_BUTTONS_NUM = 3;
	constexpr size_t PAUSE_BOTTONS_NUM = 3;

	inline static const std::string PATH_FONT = "font.ttf";
	constexpr uint8_t DEFAULT_TEXT_SIZE = 30;

	constexpr uint8_t MENU_BUTTON_WIDTH = 200;
	constexpr uint8_t MENU_BUTTON_HEIGHT = 50;
	constexpr uint8_t WON_BUTTON_WIDTH = 150;
	constexpr uint8_t WON_BUTTON_HEIGHT = 40;
	constexpr uint8_t LOSE_BUTTON_WIDTH = 150;
	constexpr uint8_t LOSE_BUTTON_HEIGHT = 40;
	constexpr uint8_t SELECT_BUTTON_WIDTH = 200;
	constexpr uint8_t SELECT_BUTTON_HEIGHT = 50;
	inline static const std::array<Rect, SELECT_BUTTONS_NUM> SELECT_BUTTONS_RECT = {
		Rect{ 64, 160, 256, 256 },
		Rect{ 64 + 256 + 64, 160, 256, 256 },
		Rect{ 64 + 256 + 64 + 256 + 64, 160, 256, 256 }
	};
	inline static const std::array<Rect, SELECT_BUTTONS_NUM> SELECT_SHOW_RECT = {
		Rect{ 64 + 64, 160 + 64, 128, 128},
		Rect{ 64 + 256 + 64 + 64, 160 + 64, 128, 128 },
		Rect{ 64 + 256 + 64 + 256 + 64 + 64, 160 + 64, 128, 128 }
	};

	constexpr uint8_t PLAYER_WIDTH = 32;
	constexpr uint8_t PLAYER_HEIGHT = 32;

	constexpr uint8_t ENEMY_WIDTH = 32;
	constexpr uint8_t ENEMY_HEIGHT = 32;
	constexpr size_t ENEMYPOOL_SIZE = 10;

	constexpr uint8_t ATTACK_WIDTH = 32;
	constexpr uint8_t ATTACK_HEIGHT = 32;

	constexpr double DELTAFREAM = 0.016f; // 0.016秒对应60帧每秒的更新频率
	constexpr uint32_t TARGET_RENDER_FPS = 120;

	constexpr float GRAVITY = 500.0f; // 重力加速度，单位像素/秒²
	constexpr float JUMP_VELOCITY = -320.0f; // 玩家跳跃初始速度，单位像素/秒
	constexpr uint8_t TILE_SIZE = 32; // 瓦片大小，单位像素
	constexpr float EPSILON = 0.001f; // 碰撞检测中的微小偏移量，避免浮点数精度问题

	constexpr float ATTACK_DURATION = 0.3f; // 攻击持续时间，单位秒
	constexpr float SPRINT_DURATION = 0.2f; // 冲刺持续时间，单位秒
	constexpr float DROP_DOWN_BUFFER_TIME = 0.5f; // 下降持续时间，单位秒
	constexpr float HIT_TIMER = 1.5f;
	constexpr float CELEBRATE_DURATION = 3.0f; // 庆祝状态持续时间，单位秒

	constexpr float ACCELERATION = 1000.0f; // 玩家水平加速度，单位像素/秒² 
	constexpr float DECELERATION = 1500.0f; // 玩家水平减速度，单位像素/秒² 
	constexpr float AIR_ACCEL = 500.0f; // 玩家在空中时的水平加速度，单位像素/秒²
	constexpr float MAX_SPEED = 275.0f; // 玩家最大水平速度，单位像素/秒
	constexpr float SPRINT_MAX_SPEED = MAX_SPEED * 2.0f; // 冲刺时的最大水平速度，单位像素/秒
	constexpr float GROUND_FRICTION = 1500.0f; // 玩家在地面上的摩擦力，单位像素/秒²
	constexpr float MAX_FALL_SPEED_WHILE_DOWN = 500.0f; // 玩家快速下落时的额外加速度，单位像素/秒²
	constexpr float PLAYER_HITBACK_VELOCITY = 1000.0f; // 玩家被击退的水平速度，单位像素/秒
	constexpr int32_t   PLAYER_HP = 5; // 玩家初始生命值
	constexpr float BLOOD_RECT_WIDTH = PLAYER_WIDTH; // 血条宽度，单位像素
	constexpr float BLOOD_RECT_HEIGHT = 5.0f; // 血条高度，单位像素

	constexpr float ENEMY_MAX_SPEED = 100.0f; // 敌人最大水平速度，单位像素/秒
	constexpr float ENEMY_MAX_FALL_SPEED = 300.0f; // 敌人最大下落速度，单位像素/秒
	constexpr float ENEMY_SCAN_DISTANCE = 500.0f; // 敌人最大扫描距离
	constexpr float ENEMY_TRACKING_COOLDOWN = 2.0f; // 敌人跟踪玩家的冷却时间，单位秒
	constexpr float ENEMY_DEAD_DURATION = 0.4f; // 敌人死亡持续时间，单位秒
	constexpr float ENEMY_HITBACK_VELOCITY = 500.0f; // 敌人被击退的水平速度，单位像素/秒
	constexpr int32_t ENEMY_HP = 3; // 敌人初始生命值
	constexpr float ENEMY_HIT_DURATION = 0.5f; // 敌人被击中持续时间，单位秒
	constexpr float ENEMY_HIT_INVINCIBILITY_DURATION = ATTACK_DURATION; // 敌人被击中后无敌持续时间，单位秒

	constexpr float JUMP_BUFFER_TIME = 0.2f; // 跳跃缓冲时间，单位秒
	constexpr float COYOTE_TIME = 0.1f; // coyote time（土狼时间），单位秒

	inline static const std::array<std::string, 3> LEVEL_PATH = {
		"resource/level1.tmx",
		"resource/level2.tmx",
	};

	constexpr uint8_t TILE_SRC_WIDTH = 16;
	constexpr uint8_t TILE_SRC_HEIGHT = 16;

	constexpr uint8_t PLAYER_1_Y_OFFSET = 0;
	constexpr uint8_t PLAYER_2_Y_OFFSET = 32;
	constexpr uint8_t PLAYER_3_Y_OFFSET = 32 * 2;

	inline static const std::array<size_t, 4> PLAYER_IDLE_SRC = { 0,0, 32 - 1, 32 - 1 }; // 玩家静止状态下的动画帧在纹理图中的位置和大小
	inline static const std::array<std::array<size_t, 4>, 4> PLAYER_RUN_SRC = { {
		{ 32, 0, 32 - 1, 32 - 1 }, // 玩家跑动动画帧1在纹理图中的位置和大小
		{ 64, 0, 32 - 1, 32 - 1 }, // 玩家跑动动画帧2在纹理图中的位置和大小
		{ 96, 0, 32 - 1, 32 - 1 }, // 玩家跑动动画帧3在纹理图中的位置和大小
		{ 0, 0, 32 - 1, 32 - 1 }, // 玩家跑动动画帧4在纹理图中的位置和大小
	} };
	inline static const std::array<std::array<size_t, 4>, 2> PLAYER_JUMP_SRC = { {
		{ 32 * 4, 0, 32 - 1, 32 - 1 }, // 玩家跳跃动画帧1在纹理图中的位置和大小 // 起跳帧
		{ 32 * 5, 0, 32 - 1, 32 - 1 }, // 玩家跳跃动画帧2在纹理图中的位置和大小	// 上升帧
	} };
	inline static const std::array<std::array<size_t, 4>, 2> PLAYER_FALL_SRC = { {
		{ 32 * 6, 0, 32 - 1, 32 - 1 }, // 玩家跳跃动画帧3在纹理图中的位置和大小 // 最高点帧
		{ 32 * 7, 0, 32 - 1, 32 - 1 }, // 玩家跳跃动画帧4在纹理图中的位置和大小 // 下降帧
	} };
	inline static const std::array<std::array<size_t, 4>, 4> PLAYER_ATTACK_SRC = { {
		{ 32 * 11, 0, 32 - 1, 32 - 1 }, // 玩家攻击动画帧1在纹理图中的位置和大小
		{ 32 * 10, 0, 32 - 1, 32 - 1 }, // 玩家攻击动画帧2在纹理图中的位置和大小
		{ 32 * 11, 0, 32 - 1, 32 - 1 }, // 玩家攻击动画帧3在纹理图中的位置和大小
		{ 32 * 12, 0, 32 - 1, 32 - 1 }, // 玩家攻击动画帧4在纹理图中的位置和大小
	} };
	inline static const std::array<std::array<size_t, 4>, 3> PLAYER_SPRINT_SRC = { {
		{ 32 * 11, 0, 32 - 1, 32 - 1 }, // 玩家冲刺动画帧1在纹理图中的位置和大小
		{ 32 * 13, 0, 32 - 1, 32 - 1 }, // 玩家冲刺动画帧2在纹理图中的位置和大小
		{ 32 * 11, 0, 32 - 1, 32 - 1 }, // 玩家冲刺动画帧3在纹理图中的位置和大小
	} };
	inline static const std::array<std::array<size_t, 4>, 3> PLAYER_HITED_SRC = { {
		{ 32 * 8, 0, 32 - 1, 32 - 1 }, // 玩家冲刺动画帧1在纹理图中的位置和大小
		{ 32 * 9, 0, 32 - 1, 32 - 1 }, // 玩家冲刺动画帧2在纹理图中的位置和大小
		{ 32 * 8, 0, 32 - 1, 32 - 1 }, // 玩家冲刺动画帧3在纹理图中的位置和大小
	} };
	inline static const std::array<std::array<size_t, 4>, 4> PLAYER_CELEBRATE_SRC = { {
		{ 32 * 14, 0, 32 - 1, 32 - 1 }, // 玩家庆祝动画帧1在纹理图中的位置和大小
		{ 32 * 15, 0, 32 - 1, 32 - 1 }, // 玩家庆祝动画帧2在纹理图中的位置和大小
		{ 32 * 16, 0, 32 - 1, 32 - 1 }, // 玩家庆祝动画帧3在纹理图中的位置和大小
		{ 32 * 17, 0, 32 - 1, 32 - 1 }, // 玩家庆祝动画帧4在纹理图中的位置和大小
	} };
	inline static const std::array<std::array<size_t, 4>, 4> ATTACK_SRC = { {
		{ 0, 0, 32, 32 }, // 攻击效果动画帧1在纹理图中的位置和大小
		{ 32, 0, 32 , 32}, // 攻击效果动画帧2在纹理图中的位置和大小
		{ 64, 0, 32 , 32 }, // 攻击效果动画帧3在纹理图中的位置和大小
		{ 96, 0, 32 , 32}, // 攻击效果动画帧4在纹理图中的位置和大小
	} };
	inline static const std::array<size_t, 4> ENEMY_IDLE_SRC = { 0, 96, 32 - 1, 32 - 1 };// 敌人静止动画帧1在纹理图中的位置和大小
	inline static const std::array<std::array<size_t, 4>, 4> ENEMY_RUN_SRC = { {
		{ 32, 96, 32 - 1, 32 - 1 }, // 敌人跑动动画帧1在纹理图中的位置和大小
		{ 64, 96, 32 - 1, 32 - 1 }, // 敌人跑动动画帧2在纹理图中的位置和大小
		{ 96, 96, 32 - 1, 32 - 1 }, // 敌人跑动动画帧3在纹理图中的位置和大小
		{ 0, 96, 32 - 1, 32 - 1 }, // 敌人跑动动画帧4在纹理图中的位置和大小
	} };

	constexpr uint8_t SPRINT_MAX_COUNT = 1; // 玩家最大连续冲刺次数，后续可以根据实际设计调整

	constexpr size_t MAX_AUDIO_SFX_NUMS = 8; // 音效数量
	constexpr uint16_t SELECT_BUTTONS_WIDTH = 256;
}


