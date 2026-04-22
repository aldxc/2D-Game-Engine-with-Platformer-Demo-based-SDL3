#include "PlayingState.h"
#include "MapLoader.h"
#include "render/Renderer.h"
#include "core/Context.h"
#include "input/Input.h"
#include "resource/Resource.h"
#include "physics/Physics.h"
#include "core/EventManager.h"
#include "UIType.h"
#include "GameSession.h"

PlayingState::PlayingState(RenderContext& renderContext, Physics& pM, Input& iM, EventManager& eM, Resource& rM, GameSession& gS) noexcept : State<StateType>(StateType::PLAYING), renderContext_(renderContext), physics_(pM) , inputManager_(iM), eventManager_(eM), resourceManager_(rM), gameSession_(gS) {
	currentLevel_ = 0; // 初始关卡编号，后续增加关卡管理等功能

	player_ = std::make_unique<Player>(renderContext_.renderer, resourceManager_); //状态开始时创建玩家实例
	enemyManager_ = std::make_unique<EnemyManager>(renderContext.renderer, rM, Config::ENEMYPOOL_SIZE); // 敌人管理器初始化
	player_->setPlayerID(gameSession_.getPlayerSkin());

	physics_.registerRigidBody(player_->getRigidBody()); // 将玩家的刚体注册到物理引擎中

	loadLevel(Config::LEVEL_PATH[gameSession_.getCurrentLevel()]);

	for(auto& obj : tileMap_->getObjects()) {
		if(obj.data == "birth") {
			player_->setBirthPoint(obj.rect); // 设置玩家出生点
			obj.isAcitive = true; // 激活出生点对象
			//SDL_Log("Birth point set at: (%f, %f)", obj.rect.x(), obj.rect.y());
		}
	}
	player_->reset(); // 将玩家重置到出生点位置

	debugSubscriptionId_ = eventManager_.subscribe(
		EventType::Debug_TogglePlayerInfo,
		[this](const Event&) {
			showPlayerDebugInfo_ = !showPlayerDebugInfo_;
		});

	renderContext_.camera.setViewport(0, 0, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT); // 设置摄像机视口为逻辑尺寸，后续增加动态调整视口大小等功能
	renderContext_.camera.setWorldBounds(0, 0, tileMap_->getMapWidth() * Config::TILE_SIZE, tileMap_->getMapHeight() * Config::TILE_SIZE);
}

PlayingState::~PlayingState() noexcept {
	eventManager_.unsubscribe(debugSubscriptionId_);
	physics_.unregisterRigidBody(player_->getRigidBody()); // 状态结束时将玩家的刚体从物理引擎中注销
	for(const auto& enemy : enemyManager_->getActiveEnemies()) {
		physics_.unregisterRigidBody(enemy->getRigidBody()); // 状态结束时将敌人的刚体从物理引擎中注销
	}
}

void PlayingState::render() const noexcept{
	renderContext_.renderer.clearStaticTexture(); // 切换到静态纹理渲染目标并清除内容，准备渲染地图等静态元素
	tileMap_->render(renderContext_.camera);

	renderContext_.renderer.clearDynamicTexture(); // 切换到动态纹理渲染目标并清除内容，准备渲染玩家等动态元素

	for (const auto& enemy : enemyManager_->getActiveEnemies()) {
		enemy->render(renderContext_.camera);
	}
	player_->render(renderContext_.camera);

	if (showPlayerDebugInfo_) {
		player_->renderDebug(renderContext_.camera);
		//for(const auto& enemy : enemyManager_->getActiveEnemies()) {
		//	enemy->renderDebug(renderContext_.camera);
		//}
	}
}

void PlayingState::update(double dt) noexcept{
	// 加入敌人
	for (auto& obj : tileMap_->getObjects()) {
		if (!renderContext_.camera.isVisible(Vec2{ obj.rect.x(), obj.rect.y() }) || obj.isAcitive) {
			continue;
		}
		if (obj.data == "enemy") {
			obj.isAcitive = true; // 激活敌人出生点对象，防止重复生成敌人
			enemyManager_->spawnEnemy(Rect{obj.rect.x(), obj.rect.y(), Config::ENEMY_WIDTH, Config::ENEMY_HEIGHT });
			auto* enemy = enemyManager_->getActiveEnemies().back();
			if (!enemy->getRegisteredToPhysics()) {
				physics_.registerRigidBody(enemyManager_->getActiveEnemies().back()->getRigidBody());
				enemy->setRegisteredToPhysics(true);
			}
		}else if(obj.data == "box") {
			obj.isAcitive = true; // 激活箱子对象，后续增加箱子被触发后的效果等功能
			tileMap_->addAnimationFrame(obj.rect.x(), obj.rect.y(), obj.animationFrames);
		}else if(obj.data == "win") {
			winPosition_ = Vec2{ obj.rect.x(), obj.rect.y() }; // 设置胜利条件位置，后续增加更复杂的胜利条件等功能
			obj.isAcitive = true; // 激活胜利条件对象，防止重复设置
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
	player_->judgeClimb(tileMap_->isCanClimb(player_->getRigidBody().hitBox)); // 根据当前碰撞盒位置判断是否可以攀爬，并更新玩家的移动模式
	for (auto& enemy : enemyManager_->getActiveEnemies()) {
		enemy->setFacingRight(player_->getRigidBody().hitBox, tileMap_->getPhysicalCollisionMap()); // 根据玩家位置和地图碰撞信息调整敌人朝向，防止敌人卡在墙壁或掉落
       if (enemy->IsDead() && enemy->getRegisteredToPhysics()) {
			physics_.unregisterRigidBody(enemy->getRigidBody()); // 将失活的敌人从物理引擎中注销，后续增加敌人死亡后的动画、掉落等功能
			enemy->setRegisteredToPhysics(false);
		}
		enemy->updateAnimationState(dt);
	}

	enemyManager_->update(dt); // 更新敌人管理器，处理敌人的生成、销毁等

	player_->update(dt);

	physics_.update(dt); // 更新物理系统，处理玩家的运动和碰撞

	resolveInteractions();
	dropDead(); // 清理掉出地图外的敌人和玩家

	if (!player_->getWin()) {
		physics_.resolveCollisions(tileMap_->getPhysicalCollisionMap(), dt, Config::EPSILON); // 处理物理系统中的碰撞检测和响应，根据地图的碰撞数据进行修正
	}

	player_->postPhysicsUpdate(); // 在物理系统更新后调用，处理玩家状态的最终调整
	player_->updateAnimationState(dt);

	//test 切换关卡
	//if (player_->getHitBox().x > 800) {
	//	loadLevel(Config::LEVEL_PATH[++currentLevel_]);
	//}

	renderContext_.camera.followTarget(player_->getRigidBody().hitBox, dt, true); // 摄像机跟随玩家，启用平滑跟随
	tileMap_->update(dt); // 更新地图状态，例如动画帧等


	winTimer_.update(dt); // 更新胜利计时器，处理胜利后玩家进行动作的时间控制

	if(player_->getWin() && !winTimer_.isActive()) {
		gameSession_.setCurrentLevel(currentLevel_ + 1); // 更新游戏会话中的当前关卡编号
		eventManager_.sendEvent(Event{ EventType::State_Transition, StateRequest{StateOperator::Replace, StateType::WON} }); // 发送事件请求切换到胜利状态
		eventManager_.sendEvent(Event{ EventType::UI_Show, UIType::WON }); // 发送事件请求显示胜利界面
	}

	resolveSfx(player_->getSfxToPlay()); // 处理玩家当前帧需要播放的音效
}

void PlayingState::dropDead() noexcept{
	for (auto& enemy : enemyManager_->getActiveEnemies()) {
		if (enemy->getRigidBody().hitBox.y() + enemy->getRigidBody().hitBox.h() > tileMap_->getMapHeight() * Config::TILE_SIZE) {
			enemy->kill(); // 将掉出地图外的敌人标记为死亡
		}
	}
	if(player_->getRigidBody().hitBox.y() + player_->getRigidBody().hitBox.h() > tileMap_->getMapHeight() * Config::TILE_SIZE) {
		player_->dead(); // 将掉出地图外的玩家标记为死亡
	}
	if(player_->isDead()) {
		eventManager_.sendEvent(Event{ EventType::State_Transition, StateRequest{StateOperator::Replace, StateType::LOSE} });
		eventManager_.sendEvent(Event{ EventType::UI_Show, UIType::LOSE });
	}
}

void PlayingState::loadLevel(const std::string& filePath){
	// 这里可以添加实际的地图加载逻辑，例如从文件中读取地图数据并解析成TileMap对象
	// 目前只是创建一个新的TileMap实例并调用loadFromFile方法作为示例
	tileMap_ = Maploader::loadMap(filePath, resourceManager_, renderContext_.renderer);
	player_->reset(); // 加载新关卡时重置玩家状态和位置
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
		winTimer_.start(Config::CELEBRATE_DURATION); // 启动胜利计时器，允许玩家在胜利后进行一些动作，例如庆祝等，后续增加胜利后玩家动作的处理等功能
	}
	if (tileMap_->isInTriggerArea(player_->getRigidBody().hitBox)) {
		// 进入触发区域，后续增加触发事件等功能
		player_->setFaced(true); // 进入触发区域时强制朝右
		player_->setHit(); // 进入触发区域时设置受击状态
		player_->getRigidBody().velocity.setX(-Config::PLAYER_HITBACK_VELOCITY); // 进入触发区域时强制停止
	}
	if (tileMap_->isInDeadArea(player_->getRigidBody().hitBox)) {
		player_->dead(); // 进入死亡区域时标记玩家为死亡
	}
}

void PlayingState::resolveSfx(std::vector<SfxId>& sfxs){
	while (!sfxs.empty()) {
		auto sfxId = sfxs.back();
		sfxs.pop_back();
		eventManager_.sendEvent(Event{ EventType::Audio_PlaySfx, sfxId }); // 发送事件请求播放音效
	}
}
