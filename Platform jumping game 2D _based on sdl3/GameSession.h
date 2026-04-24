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
	void setCurrentLevel(size_t level) noexcept { m_currentLevel = level; } 
	void setPlayerSkinIndex(size_t index) noexcept { m_playerSkinIndex = index; }
	void reset() noexcept; 
	size_t getCurrentLevel() const noexcept { return m_currentLevel; }
	size_t getPlayerSkin() const noexcept { return m_playerSkinIndex; }
private:
	// ¥Êµµ π”√
	Resource& m_resourceManager; 

	size_t m_currentLevel = 0; 
	size_t m_playerSkinIndex = 0; 
	int32_t m_playerHp = Config::PLAYER_HP;
	int32_t m_playerMaxHp = Config::PLAYER_HP;
};