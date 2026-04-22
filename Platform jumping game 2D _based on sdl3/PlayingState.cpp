#include "PlayingState.h"
#include "MapLoader.h"
#include "render/Renderer.h"
#include "render/Camera.h"
#include "core/Context.h"
#include "input/Input.h"
#include "resource/Resource.h"
#include "physics/Physics.h"
#include "core/EventManager.h"
#include "Player.h"
#include "EnemyManager.h"
#include "UIType.h"
#include "GameSession.h"

PlayingState::PlayingState(RenderContext& renderContext, Physics& pM, Input& iM, EventManager& eM, Resource& rM, GameSession& gS) noexcept : State<StateType>(StateType::PLAYING), renderContext_(renderContext), physics_(pM) , inputManager_(iM), eventManager_(eM), resourceManager_(rM), gameSession_(gS) {
	currentLevel_ = 0; 

	player_ = std::make_unique<Player>(renderContext_.renderer, resourceManager_); 
	player_->setPlayerID(gameSession_.getPlayerSkin());
	// 将玩家的刚体注册到物理引擎中
	physics_.registerRigidBody(player_->getRigidBody()); 

	enemyManager_ = std::make_unique<EnemyManager>(renderContext.renderer, rM, Config::ENEMYPOOL_SIZE);

	// 读取地图数据创建TileMap对象，并重置玩家位置
	loadLevel(Config::LEVEL_PATH[gameSession_.getCurrentLevel()]);

	debugSubscriptionId_ = eventManager_.subscribe(
		EventType::DEBUG_TOGGLE_PLAYER_INFO,
		[this](const Event&) {
			showPlayerDebugInfo_ = !showPlayerDebugInfo_;
		});

	renderContext_.camera.setViewport(0, 0, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT); 
	renderContext_.camera.setWorldBounds(0, 0, tileMap_->getMapWidth() * Config::TILE_SIZE, tileMap_->getMapHeight() * Config::TILE_SIZE);
}

PlayingState::~PlayingState() noexcept {
	eventManager_.unsubscribe(debugSubscriptionId_);
	// 注销玩家和敌人的刚体，确保它们不再参与物理计算
	physics_.unregisterRigidBody(player_->getRigidBody()); 
	for(const auto& enemy : enemyManager_->getActiveEnemies()) {
		physics_.unregisterRigidBody(enemy->getRigidBody()); 
	}
}

void PlayingState::render() const noexcept{
	renderContext_.renderer.clearStaticTexture(); 
	tileMap_->render(renderContext_.camera);

	renderContext_.renderer.clearDynamicTexture(); 

	for (const auto& enemy : enemyManager_->getActiveEnemies()) {
		enemy->render(renderContext_.camera);
	}
	player_->render(renderContext_.camera);

	if (showPlayerDebugInfo_) {
		player_->renderDebug(renderContext_.camera);
	}
}

void PlayingState::update(double dt) noexcept{
	// 加入敌人
	for (auto& obj : tileMap_->getObjects()) {
		if (!renderContext_.camera.isVisible(Vec2{ obj.rect.x(), obj.rect.y() }) || obj.isAcitive) {
			continue;
		}
		if (obj.data == "enemy") {
			obj.isAcitive = true; 
			enemyManager_->spawnEnemy(Rect{obj.rect.x(), obj.rect.y(), Config::ENEMY_WIDTH, Config::ENEMY_HEIGHT });
			auto* enemy = enemyManager_->getActiveEnemies().back();
			if (!enemy->getRegisteredToPhysics()) {
				physics_.registerRigidBody(enemyManager_->getActiveEnemies().back()->getRigidBody());
				enemy->setRegisteredToPhysics(true);
			}
		}else if(obj.data == "box") {
			obj.isAcitive = true; 
			tileMap_->addAnimationFrame(obj.rect.x(), obj.rect.y(), obj.animationFrames);
		}else if(obj.data == "win") {
			winPosition_ = Vec2{ obj.rect.x(), obj.rect.y() }; 
			obj.isAcitive = true; 
		}
	}

	// 顺序： 输入移动 - 重力 - 碰撞修正
	player_->setCommand(Player::PlayerCommand{
		inputManager_.getMoveLeftPressed(),
		inputManager_.getMoveRightPressed(),
		inputManager_.getJumpPressed(),
		inputManager_.getAttackPressed(),
		inputManager_.getUPPressed(),
		inputManager_.getDownPressed(),
		inputManager_.getSprintPressed(),
		inputManager_.getFlashPressed()
		});
	// 根据当前碰撞盒位置判断是否可以攀爬，并更新玩家的移动模式
	player_->judgeClimb(tileMap_->isCanClimb(player_->getRigidBody().hitBox)); 

	for (auto& enemy : enemyManager_->getActiveEnemies()) {
		// 根据玩家位置和地图碰撞信息调整敌人朝向，防止敌人卡在墙壁或掉落
		enemy->setFacingRight(player_->getRigidBody().hitBox, tileMap_->getPhysicalCollisionMap()); 
        if (enemy->IsDead() && enemy->getRegisteredToPhysics()) {
			// 将失活的敌人从物理引擎中注销
			physics_.unregisterRigidBody(enemy->getRigidBody()); 
			enemy->setRegisteredToPhysics(false);
		}
		enemy->updateAnimationState(dt);
	}

	// 更新敌人管理器，处理敌人的生成、销毁等
	enemyManager_->update(dt); 

	player_->update(dt);

	// 更新物理系统，处理玩家的运动和碰撞
	physics_.update(dt); 

	resolveInteractions();

	if (!player_->getWin()) {
		// 处理物理系统中的碰撞检测和响应，根据地图的碰撞数据进行修正
		physics_.resolveCollisions(tileMap_->getPhysicalCollisionMap(), dt, Config::EPSILON); 
	}

	// 在物理系统更新后调用，处理玩家状态的最终调整
	player_->postPhysicsUpdate(); 
	player_->updateAnimationState(dt);

	// 摄像机跟随玩家，启用平滑跟随
	renderContext_.camera.followTarget(player_->getRigidBody().hitBox, dt, true); 
	tileMap_->update(dt); 
	// 更新胜利计时器，处理胜利后玩家进行动作的时间控制
	winTimer_.update(dt); 

	if(player_->getWin() && !winTimer_.isActive()) {
		// 更新游戏会话中的当前关卡编号
		gameSession_.setCurrentLevel(currentLevel_ + 1); 
		eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::WON} }); 
		eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::WON }); 
	}

	// 处理玩家当前帧需要播放的音效
	resolveSfx(player_->getSfxToPlay()); 
}

void PlayingState::dropDead() noexcept{
	// 将掉出地图外的标记为死亡
	for (auto& enemy : enemyManager_->getActiveEnemies()) {
		if (enemy->getRigidBody().hitBox.y() + enemy->getRigidBody().hitBox.h() > tileMap_->getMapHeight() * Config::TILE_SIZE) {
			enemy->kill(); 
		}
	}
	if(player_->getRigidBody().hitBox.y() + player_->getRigidBody().hitBox.h() > tileMap_->getMapHeight() * Config::TILE_SIZE) {
		player_->dead(); 
	}
	if(player_->isDead()) {
		eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::LOSE} });
		eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::LOSE });
	}
}

void PlayingState::loadLevel(const std::string& filePath){
	tileMap_ = Maploader::loadMap(filePath, resourceManager_, renderContext_.renderer);
	for (auto& obj : tileMap_->getObjects()) {
		if (obj.data == "birth") {
			player_->setBirthPoint(obj.rect);
			obj.isAcitive = true;
		}
	}
	player_->reset(); // 将玩家重置到出生点位置
}

void PlayingState::resolveInteractions(){
	// 简单的碰撞检测逻辑，后续增加更复杂的攻击判定和响应等功能
	for (auto& enemy : enemyManager_->getActiveEnemies()) {
		// 碰撞玩家
		if(physics_.isColliding(player_->getRigidBody().hitBox, enemy->getRigidBody().hitBox) && !enemy->IsDead() && player_->getIsCollidable()) {
			// 受击强制后退，无敌时间，后续添加掉血
			int dir = player_->getRigidBody().hitBox.x() < enemy->getRigidBody().hitBox.x()  ? -1 : 1; 
			player_->setHit();
			player_->setFaced(dir < 0); // 根据敌人位置调整玩家朝向
			player_->getRigidBody().acceleration.setX(0); // 强制停止
			player_->getRigidBody().velocity.setX(dir * Config::PLAYER_HITBACK_VELOCITY);
		}
		// 杀死敌人
		if (player_->isAttacking() && !enemy->IsDead()) {
			if (physics_.isColliding(player_->getAttackHitBox(), enemy->getRigidBody().hitBox)) {
				int dir = player_->getRigidBody().hitBox.x() < enemy->getRigidBody().hitBox.x() ? 1 : -1;
				enemy->takeHit(dir); // 设置敌人受击状态，后续增加敌人血量、死亡等功能
				resolveSfx(enemy->getSfxToPlay());
			}
		}
	}
	for (auto& box : tileMap_->getAnimationFrames()) {
		if (!box.second.second.isActive() && physics_.isColliding(player_->getRigidBody().hitBox, Rect{ static_cast<float>(box.first) / tileMap_->getMapCol(), static_cast<float>(box.first % tileMap_->getMapCol()), Config::TILE_SIZE, Config::TILE_SIZE })) {
			double totalTime = 0.0;
			for (auto& frame : box.second.first) {
				totalTime += frame.second / 1000.0; // 将持续时间从毫秒转换为秒
			}
			box.second.second.start(totalTime * 1.2);
		}
	}
	if (player_->getRigidBody().hitBox.hasIntersection(winPosition_) && !winTimer_.isActive()) {
		player_->setWin(); // 设置玩家为胜利状态，后续增加胜利动画等功能
		// 启动胜利计时器，玩家在胜利后进行一些动作
		winTimer_.start(Config::CELEBRATE_DURATION); 
	}
	if (tileMap_->isInTriggerArea(player_->getRigidBody().hitBox)) {
		// 进入触发区域，后续增加触发事件等功能
		player_->setFaced(true); 
		player_->setHit(); 
		player_->getRigidBody().velocity.setX(-Config::PLAYER_HITBACK_VELOCITY); 
	}
	if (tileMap_->isInDeadArea(player_->getRigidBody().hitBox)) {
		// 进入死亡区域时标记玩家为死亡
		player_->dead(); 
	}

	// 处理掉出地图外的玩家和敌人
	dropDead(); 
}

void PlayingState::resolveSfx(std::vector<SfxId>& sfxs){
	while (!sfxs.empty()) {
		auto sfxId = sfxs.back();
		sfxs.pop_back();
		eventManager_.sendEvent(Event{ EventType::AUDIO_PLAY_SFX, sfxId }); 
	}
}
