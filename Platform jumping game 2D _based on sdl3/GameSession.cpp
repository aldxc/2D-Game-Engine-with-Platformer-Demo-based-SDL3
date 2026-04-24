#include "resource/Resource.h"
#include "GameSession.h"

GameSession::GameSession(Resource& rM) noexcept : m_resourceManager(rM){
}

bool GameSession::init() noexcept{

    return true;
}

bool GameSession::loadLevel() noexcept{
    return false;
}

void GameSession::saveLevel() noexcept{
 //   uint32_t data = m_currentLevel << 0 | m_playerSkinIndex << 8 | m_playerHp << 16 | m_playerMaxHp << 24; // 将当前关卡编号、玩家皮肤索引、玩家生命值和玩家最大生命值打包成一个32位整数，后续增加更多的存档数据时可以继续使用位运算进行打包
	//m_resourceManager.saveGameData(levelFilePath_, data); // 将打包后的数据保存到文件中，后续增加更多的存档数据时可以继续使用位运算进行打包
}

void GameSession::reset() noexcept {
    m_currentLevel = 0;
    m_playerSkinIndex = 0;
	m_playerHp = Config::PLAYER_HP;
}

