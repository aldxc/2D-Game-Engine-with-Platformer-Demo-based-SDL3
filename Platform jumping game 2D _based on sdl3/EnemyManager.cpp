#include "EnemyManager.h"

EnemyManager::EnemyManager(Renderer& renderer, Resource& rM, size_t poolSize) noexcept
	: enemyPool_(poolSize, [&]() {return std::make_unique<Enemy>(renderer, rM); }) {
	
}

void EnemyManager::update(double dt) noexcept {
	for (Enemy* enemy : activeEnemies_) {
		enemy->update(dt);
	}
	auto it = activeEnemies_.begin();
	while (it != activeEnemies_.end()) { 
		if ((*it)->getIsDestroyed()) { // 死亡且计时器结束，敌人可以被重置或销毁
			enemyPool_.release(*it);
			it = activeEnemies_.erase(it); // 从活跃列表中移除敌人
		}
		else {
			++it;
		}
	}
}

void EnemyManager::spawnEnemy(const Rect& enemyInfo) noexcept{
	Enemy* enemy = enemyPool_.acquire();
	if (enemy) {
		// 重置敌人状态，例如位置、动画状态等并将敌人添加到活跃列表中
		enemy->reset(enemyInfo); 
		activeEnemies_.push_back(enemy); 
	} else {
		SDL_Log("Warning: Enemy pool exhausted, cannot spawn new enemy.");
	}
}
