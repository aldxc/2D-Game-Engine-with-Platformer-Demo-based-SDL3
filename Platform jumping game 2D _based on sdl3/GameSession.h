#pragma once
#include <string>
#include <cstdint>
#include "Config.h"

class Resource;

class GameSession {
public:
	explicit GameSession(Resource& rM) noexcept;
	~GameSession() noexcept = default;
	bool init() noexcept; // 初始化游戏会话
	bool loadLevel() noexcept; // 从存档文件加载当前关卡数据
	void saveLevel() noexcept; // 将当前关卡数据保存到存档文件
	void setCurrentLevel(size_t level) noexcept { currentLevel_ = level; } // 设置当前关卡编号
	void setPlayerSkinIndex(size_t index) noexcept { playerSkinIndex_ = index; } // 设置玩家皮肤索引
	void reset() noexcept; // 重置数据
	size_t getCurrentLevel() const noexcept { return currentLevel_; }
	size_t getPlayerSkin() const noexcept { return playerSkinIndex_; }
private:
	Resource& resourceManager_; // 存档使用

	std::string levelFilePath_ = "saves/data.bin"; // 当前关卡文件路径
	size_t currentLevel_ = 0; // 当前关卡编号，后续增加关卡管理等功能
	size_t playerSkinIndex_ = 0; // 玩家皮肤索引
	int32_t playerHp_ = Config::PLAYER_HP;
	int32_t playerMaxHp_ = Config::PLAYER_HP;
};