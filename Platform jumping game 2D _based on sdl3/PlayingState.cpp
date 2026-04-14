#include "PlayingState.h"
#include "MapLoader.h"
#include "render/Renderer.h"
#include "core/Context.h"
#include "input/Input.h"
#include "resource/Resource.h"
#include "physics/Physics.h"
#include "core/EventManager.h"
#include "UIType.h"

PlayingState::PlayingState(RenderContext& renderContext, Physics& pM, Input& iM, EventManager& eM, Resource& rM) noexcept : State<StateType>(StateType::PLAYING), renderContext_(renderContext), physics_(pM) , inputManager_(iM), eventManager_(eM), resourceManager_(rM){
	currentLevel_ = 0; // 初始关卡编号，后续增加关卡管理等功能

	player_ = std::make_unique<Player>(renderContext_.renderer, resourceManager_);//状态开始时创建玩家实例
	physics_.registerRigidBody(player_->getRigidBody()); // 将玩家的刚体注册到物理引擎中
	loadLevel("resource/leveltest.tmx");

	debugSubscriptionId_ = eventManager_.subscribe(
		EventType::Debug_TogglePlayerInfo,
		[this](const Event&) {
			showPlayerDebugInfo_ = !showPlayerDebugInfo_;
		});

	//camera_ = std::make_unique<Camera>();//状态开始时创建摄像机实例
	renderContext_.camera.setViewport(0, 0, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT); // 设置摄像机视口为逻辑尺寸，后续增加动态调整视口大小等功能
	renderContext_.camera.setWorldBounds(0, 0, tileMap_->getMapWidth() * Config::TILE_SIZE, tileMap_->getMapHeight() * Config::TILE_SIZE);
}

PlayingState::~PlayingState() noexcept {
	eventManager_.unsubscribe(debugSubscriptionId_);
	physics_.unregisterRigidBody(player_->getRigidBody()); // 状态结束时将玩家的刚体从物理引擎中注销
}

void PlayingState::render() const noexcept{
	renderContext_.renderer.clearStaticTexture(); // 切换到静态纹理渲染目标并清除内容，准备渲染地图等静态元素
	tileMap_->render(renderContext_.camera);

	renderContext_.renderer.clearDynamicTexture(); // 切换到动态纹理渲染目标并清除内容，准备渲染玩家等动态元素
	player_->render(renderContext_.camera);

	if (showPlayerDebugInfo_) {
		player_->renderDebug(renderContext_.camera);
	}
}

void PlayingState::update(float dt) noexcept{
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
	player_->update(dt);

	physics_.update(dt); // 更新物理系统，处理玩家的运动和碰撞

	physics_.resolveCollisions(tileMap_->getPhysicalCollisionMap(), dt, Config::EPSILON); // 处理物理系统中的碰撞检测和响应，根据地图的碰撞数据进行修正

	// 碰撞检测和修正
	//const TileMap::CollisionResult result = tileMap_->tileCollision(player_->getHitBox(), player_->getVelocityX(), player_->getVelocityY(), player_->isLanded(), dt);
	// 
	//player_->applyResolvedMovement(result.hitbox, result.velocityX, result.velocityY, result.isLanded);

	player_->postPhysicsUpdate(); // 在物理系统更新后调用，处理玩家状态的最终调整
	player_->updateAnimationState(dt);

	//test 切换关卡
	//if (player_->getHitBox().x > 800) {
	//	loadLevel(Config::LEVEL_PATH[++currentLevel_]);
	//}

	renderContext_.camera.followTarget(player_->getRigidBody().hitBox, dt, true); // 摄像机跟随玩家，启用平滑跟随

	//test win condition
	if(currentLevel_ == 2) {
		eventManager_.sendEvent(Event{ EventType::State_Transition, StateType::WON });
		eventManager_.sendEvent(Event{ EventType::UI_Show, UIType::WON });
	}
}

void PlayingState::loadLevel(const std::string& filePath){
	// 这里可以添加实际的地图加载逻辑，例如从文件中读取地图数据并解析成TileMap对象
	// 目前只是创建一个新的TileMap实例并调用loadFromFile方法作为示例
	tileMap_ = Maploader::loadMap(filePath, resourceManager_, renderContext_.renderer);
	player_->reset(); // 加载新关卡时重置玩家状态和位置
}
