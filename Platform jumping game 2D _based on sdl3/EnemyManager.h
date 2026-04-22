#pragma once
#include <vector>
#include "pool/ObjectPool.h"
#include "Enemy.h"

class Renderer;
class Resource;

class EnemyManager {
public:
	EnemyManager(Renderer& renderer, Resource& rM, size_t poolSize) noexcept;

	std::vector<Enemy*>& getActiveEnemies() noexcept { return activeEnemies_; }

	void update(double dt) noexcept;

	// 生成敌人，后续增加生成位置、类型等参数
	void spawnEnemy(const Rect& enemyInfo) noexcept; 

	size_t getActiveEnemyCount() const noexcept { return activeEnemies_.size(); }
private:
	ObjectPool<Enemy> enemyPool_;
	// 当前活跃的敌人列表 // 删除敌人为O(n)后续可优化
	std::vector<Enemy*> activeEnemies_ = {}; 
};