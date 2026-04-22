#pragma once
#include <string>
#include <cstdint>
#include "Config.h"

class Resource;

class GameSession {
public:
	explicit GameSession(Resource& rM) noexcept;
	~GameSession() noexcept = default;
	bool init() noexcept; 
	bool loadLevel() noexcept; 
	void saveLevel() noexcept; 
	void setCurrentLevel(size_t level) noexcept { currentLevel_ = level; } 
	void setPlayerSkinIndex(size_t index) noexcept { playerSkinIndex_ = index; }
	void reset() noexcept; 
	size_t getCurrentLevel() const noexcept { return currentLevel_; }
	size_t getPlayerSkin() const noexcept { return playerSkinIndex_; }
private:
	// ¥Êµµ π”√
	Resource& resourceManager_; 

	size_t currentLevel_ = 0; 
	size_t playerSkinIndex_ = 0; 
	int32_t playerHp_ = Config::PLAYER_HP;
	int32_t playerMaxHp_ = Config::PLAYER_HP;
};