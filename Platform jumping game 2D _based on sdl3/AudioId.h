#pragma once
#include <cstdint>
#include<unordered_map>
#include <string>

enum class SfxId : uint8_t {
    PlayerAttackSwing,
    PlayerAttackHit,
    PlayerJump,
    PlayerSprint,
    PlayerHurt,
    EnemyDeath,
    UIButtonClick,
    PauseOpen,
    PauseClose
};

enum class BgmId : uint8_t {
    Menu,
    Playing,
    Select,
    Boss,
    Win,
    Lose,
	None
};

inline static const std::unordered_map<SfxId, std::string> SfxFilePaths = {
    { SfxId::PlayerAttackSwing, "resource/sfx/attack.wav" },
    { SfxId::PlayerAttackHit, "resource/sfx/enemyhited.wav" },
    { SfxId::PlayerJump, "resource/sfx/jump.wav" },
    { SfxId::PlayerSprint, "resource/sfx/sprint.ogg" },
    { SfxId::PlayerHurt, "resource/sfx/playerhited.wav" },
    //{ SfxId::EnemyDeath, "assets/sfx/enemy_death.wav" },
    { SfxId::UIButtonClick, "resource/sfx/click.wav" },
    //{ SfxId::PauseOpen, "assets/sfx/pause_open.wav" },
    //{ SfxId::PauseClose, "assets/sfx/pause_close.wav" }
};

inline static const std::unordered_map<BgmId, std::string> BgmFilePaths = {
    { BgmId::Menu, "resource/bgm/menu.mp3" },
    { BgmId::Playing, "resource/bgm/playing.mp3" },
    { BgmId::Select, "resource/bgm/select.wav"},
    { BgmId::Boss, "assets/bgm/boss_music.mp3" },
    { BgmId::Win, "assets/bgm/win_music.mp3" },
    { BgmId::Lose, "assets/bgm/lose_music.mp3" }
};

enum class Priority : uint8_t {
    Low = 1, // 挥刀、脚步、环境小提示等
    Medium = 2, // 跳跃、冲刺、落地等
    High = 3, // 命中、击杀、拾取关键道具等
	Max = 4 // 受伤、濒死、Boss大招预警等
};