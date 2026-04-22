#pragma once
#include <cstdint>
#include<unordered_map>
#include <string>

enum class SfxId : uint8_t {
    PLAYER_ATTACK_SWING,
    PLAYER_ATTACK_HIT,
    PLAYER_JUMP,
    PLAYER_SPRINT,
    PLAYER_HURT,
    ENEMY_DEATH,
    UI_BUTTON_CLICK,
    PAUSE_OPEN,
    PAUSE_CLOSE
};

enum class BgmId : uint8_t {
    MENU,
    PLAYING,
    SELECT,
    BOSS,
    WIN,
    LOSE,
	NONE
};

inline static const std::unordered_map<SfxId, std::string> SfxFilePaths = {
    { SfxId::PLAYER_ATTACK_SWING, "resource/sfx/attack.wav" },
    { SfxId::PLAYER_ATTACK_HIT, "resource/sfx/enemyhited.wav" },
    { SfxId::PLAYER_JUMP, "resource/sfx/jump.wav" },
    { SfxId::PLAYER_SPRINT, "resource/sfx/sprint.ogg" },
    { SfxId::PLAYER_HURT, "resource/sfx/playerhited.wav" },
    { SfxId::UI_BUTTON_CLICK, "resource/sfx/click.wav" },
};

inline static const std::unordered_map<BgmId, std::string> BgmFilePaths = {
    { BgmId::MENU, "resource/bgm/menu.mp3" },
    { BgmId::PLAYING, "resource/bgm/playing.mp3" },
    { BgmId::SELECT, "resource/bgm/select.wav"},
};

enum class Priority : uint8_t {
    Low = 1, // 挥刀、脚步、环境小提示等
    Medium = 2, // 跳跃、冲刺、落地等
    High = 3, // 命中、击杀、拾取关键道具等
	Max = 4 // 受伤、濒死、BOSS大招预警等
};