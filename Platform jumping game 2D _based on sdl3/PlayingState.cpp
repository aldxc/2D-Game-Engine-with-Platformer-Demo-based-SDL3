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

PlayingState::PlayingState(RenderContext& renderContext, Physics& pM, Input& iM, EventManager& eM, Resource& rM, GameSession& gS) noexcept : State<StateType>(StateType::PLAYING), m_renderContext(renderContext), m_physics(pM) , m_inputManager(iM), m_eventManager(eM), m_resourceManager(rM), m_gameSession(gS) {
	m_currentLevel = 0; 

	m_player = std::make_unique<Player>(m_renderContext.renderer, m_resourceManager); 
	m_player->setPlayerID(m_gameSession.getPlayerSkin());
	// 将玩家的刚体注册到物理引擎中
	m_physics.registerRigidBody(m_player->getRigidBody()); 

	m_enemyManager = std::make_unique<EnemyManager>(renderContext.renderer, rM, Config::ENEMYPOOL_SIZE);

	// 读取地图数据创建TileMap对象，并重置玩家位置
	loadLevel(Config::LEVEL_PATH[m_gameSession.getCurrentLevel()]);

	m_debugSubscriptionId = m_eventManager.subscribe(
		EventType::DEBUG_TOGGLE_PLAYER_INFO,
		[this](const Event&) {
			m_showPlayerDebugInfo = !m_showPlayerDebugInfo;
		});

	m_renderContext.camera.setViewport(0, 0, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT); 
	m_renderContext.camera.setWorldBounds(0, 0, m_tileMap->getMapWidth() * Config::TILE_SIZE, m_tileMap->getMapHeight() * Config::TILE_SIZE);
}

PlayingState::~PlayingState() noexcept {
	m_eventManager.unsubscribe(m_debugSubscriptionId);
	// 注销玩家和敌人的刚体，确保它们不再参与物理计算
	m_physics.unregisterRigidBody(m_player->getRigidBody()); 
	for(const auto& enemy : m_enemyManager->getActiveEnemies()) {
		m_physics.unregisterRigidBody(enemy->getRigidBody()); 
	}
}

void PlayingState::render() const noexcept{
	m_renderContext.renderer.clearStaticTexture(); 
	m_tileMap->render(m_renderContext.camera);

	m_renderContext.renderer.clearDynamicTexture(); 

	for (const auto& enemy : m_enemyManager->getActiveEnemies()) {
		enemy->render(m_renderContext.camera);
	}
	m_player->render(m_renderContext.camera);

	if (m_showPlayerDebugInfo) {
		m_player->renderDebug(m_renderContext.camera);
	}
}

void PlayingState::update(double dt) noexcept{
	// 加入敌人
	for (auto& obj : m_tileMap->getObjects()) {
		if (!m_renderContext.camera.isVisible(Vec2{ obj.rect.x(), obj.rect.y() }) || obj.isAcitive) {
			continue;
		}
		if (obj.data == "enemy") {
			obj.isAcitive = true; 
			m_enemyManager->spawnEnemy(Rect{obj.rect.x(), obj.rect.y(), Config::ENEMY_WIDTH, Config::ENEMY_HEIGHT });
			auto* enemy = m_enemyManager->getActiveEnemies().back();
			if (!enemy->getRegisteredToPhysics()) {
				m_physics.registerRigidBody(m_enemyManager->getActiveEnemies().back()->getRigidBody());
				enemy->setRegisteredToPhysics(true);
			}
		}else if(obj.data == "box") {
			obj.isAcitive = true; 
			m_tileMap->addAnimationFrame(obj.rect.x(), obj.rect.y(), obj.animationFrames);
		}else if(obj.data == "win") {
			m_winPosition = Vec2{ obj.rect.x(), obj.rect.y() }; 
			obj.isAcitive = true; 
		}
	}

	// 顺序： 输入移动 - 重力 - 碰撞修正
	m_player->setCommand(Player::PlayerCommand{
		m_inputManager.getMoveLeftPressed(),
		m_inputManager.getMoveRightPressed(),
		m_inputManager.getJumpPressed(),
		m_inputManager.getAttackPressed(),
		m_inputManager.getUPPressed(),
		m_inputManager.getDownPressed(),
		m_inputManager.getSprintPressed(),
		m_inputManager.getFlashPressed()
		});
	// 根据当前碰撞盒位置判断是否可以攀爬，并更新玩家的移动模式
	m_player->judgeClimb(m_tileMap->isCanClimb(m_player->getRigidBody().hitBox)); 

	for (auto& enemy : m_enemyManager->getActiveEnemies()) {
		// 根据玩家位置和地图碰撞信息调整敌人朝向，防止敌人卡在墙壁或掉落
		enemy->setFacingRight(m_player->getRigidBody().hitBox, m_tileMap->getPhysicalCollisionMap()); 
        if (enemy->IsDead() && enemy->getRegisteredToPhysics()) {
			// 将失活的敌人从物理引擎中注销
			m_physics.unregisterRigidBody(enemy->getRigidBody()); 
			enemy->setRegisteredToPhysics(false);
		}
		enemy->updateAnimationState(dt);
	}

	// 更新敌人管理器，处理敌人的生成、销毁等
	m_enemyManager->update(dt); 

	m_player->update(dt);

	// 更新物理系统，处理玩家的运动和碰撞
	m_physics.update(dt); 

	resolveInteractions();

	if (!m_player->getWin()) {
		// 处理物理系统中的碰撞检测和响应，根据地图的碰撞数据进行修正
		m_physics.resolveCollisions(m_tileMap->getPhysicalCollisionMap(), dt, Config::EPSILON); 
	}

	// 在物理系统更新后调用，处理玩家状态的最终调整
	m_player->postPhysicsUpdate(); 
	m_player->updateAnimationState(dt);

	// 摄像机跟随玩家，启用平滑跟随
	m_renderContext.camera.followTarget(m_player->getRigidBody().hitBox, dt, true); 
	m_tileMap->update(dt); 
	// 更新胜利计时器，处理胜利后玩家进行动作的时间控制
	m_winTimer.update(dt); 

	if(m_player->getWin() && !m_winTimer.isActive()) {
		// 更新游戏会话中的当前关卡编号
		m_gameSession.setCurrentLevel(m_currentLevel + 1); 
		m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::WON} }); 
		m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::WON }); 
	}

	// 处理玩家当前帧需要播放的音效
	resolveSfx(m_player->getSfxToPlay()); 
}

void PlayingState::dropDead() noexcept{
	// 将掉出地图外的标记为死亡
	for (auto& enemy : m_enemyManager->getActiveEnemies()) {
		if (enemy->getRigidBody().hitBox.y() + enemy->getRigidBody().hitBox.h() > m_tileMap->getMapHeight() * Config::TILE_SIZE) {
			enemy->kill(); 
		}
	}
	if(m_player->getRigidBody().hitBox.y() + m_player->getRigidBody().hitBox.h() > m_tileMap->getMapHeight() * Config::TILE_SIZE) {
		m_player->dead(); 
	}
	if(m_player->isDead()) {
		m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::LOSE} });
		m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::LOSE });
	}
}

void PlayingState::loadLevel(const std::string& filePath){
	m_tileMap = Maploader::loadMap(filePath, m_resourceManager, m_renderContext.renderer);
	for (auto& obj : m_tileMap->getObjects()) {
		if (obj.data == "birth") {
			m_player->setBirthPoint(obj.rect);
			obj.isAcitive = true;
		}
	}
	m_player->reset(); // 将玩家重置到出生点位置
}

void PlayingState::resolveInteractions(){
	// 简单的碰撞检测逻辑，后续增加更复杂的攻击判定和响应等功能
	for (auto& enemy : m_enemyManager->getActiveEnemies()) {
		// 碰撞玩家
		if(m_physics.isColliding(m_player->getRigidBody().hitBox, enemy->getRigidBody().hitBox) && !enemy->IsDead() && m_player->getIsCollidable()) {
			// 受击强制后退，无敌时间，后续添加掉血
			int dir = m_player->getRigidBody().hitBox.x() < enemy->getRigidBody().hitBox.x()  ? -1 : 1; 
			m_player->setHit();
			m_player->setFaced(dir < 0); // 根据敌人位置调整玩家朝向
			m_player->getRigidBody().acceleration.setX(0); // 强制停止
			m_player->getRigidBody().velocity.setX(dir * Config::PLAYER_HITBACK_VELOCITY);
		}
		// 杀死敌人
		if (m_player->isAttacking() && !enemy->IsDead()) {
			if (m_physics.isColliding(m_player->getAttackHitBox(), enemy->getRigidBody().hitBox)) {
				int dir = m_player->getRigidBody().hitBox.x() < enemy->getRigidBody().hitBox.x() ? 1 : -1;
				enemy->takeHit(dir); // 设置敌人受击状态，后续增加敌人血量、死亡等功能
				resolveSfx(enemy->getSfxToPlay());
			}
		}
	}
	for (auto& box : m_tileMap->getAnimationFrames()) {
		if (!box.second.second.isActive() && m_physics.isColliding(m_player->getRigidBody().hitBox, Rect{ static_cast<float>(box.first) / m_tileMap->getMapCol(), static_cast<float>(box.first % m_tileMap->getMapCol()), Config::TILE_SIZE, Config::TILE_SIZE })) {
			double totalTime = 0.0;
			for (auto& frame : box.second.first) {
				totalTime += frame.second / 1000.0; // 将持续时间从毫秒转换为秒
			}
			box.second.second.start(totalTime * 1.2);
		}
	}
	if (m_player->getRigidBody().hitBox.hasIntersection(m_winPosition) && !m_winTimer.isActive()) {
		m_player->setWin(); // 设置玩家为胜利状态，后续增加胜利动画等功能
		// 启动胜利计时器，玩家在胜利后进行一些动作
		m_winTimer.start(Config::CELEBRATE_DURATION); 
	}
	if (m_tileMap->isInTriggerArea(m_player->getRigidBody().hitBox)) {
		// 进入触发区域，后续增加触发事件等功能
		m_player->setFaced(true); 
		m_player->setHit(); 
		m_player->getRigidBody().velocity.setX(-Config::PLAYER_HITBACK_VELOCITY); 
	}
	if (m_tileMap->isInDeadArea(m_player->getRigidBody().hitBox)) {
		// 进入死亡区域时标记玩家为死亡
		m_player->dead(); 
	}

	// 处理掉出地图外的玩家和敌人
	dropDead(); 
}

void PlayingState::resolveSfx(std::vector<SfxId>& sfxs){
	while (!sfxs.empty()) {
		auto sfxId = sfxs.back();
		sfxs.pop_back();
		m_eventManager.sendEvent(Event{ EventType::AUDIO_PLAY_SFX, sfxId }); 
	}
}
