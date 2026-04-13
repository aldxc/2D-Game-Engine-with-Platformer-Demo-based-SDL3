#include "PlayingState.h"
#include "MapLoader.h"
#include "render/Renderer.h"
#include "core/Context.h"
#include "input/Input.h"
#include "resource/Resource.h"
#include "physics/Physics.h"
#include "core/EventManager.h"
#include "UIType.h"

PlayingState::PlayingState(Context& context) noexcept : State<StateType>(StateType::PLAYING), context_(context) {
	currentLevel_ = 0; // 初始关卡编号，后续增加关卡管理等功能

	player_ = std::make_unique<Player>();//状态开始时创建玩家实例
	context_.physicsEngine.registerRigidBody(player_->getRigidBody()); // 将玩家的刚体注册到物理引擎中
	loadLevel("resource/leveltest.tmx");

	debugSubscriptionId_ = EventManager::getInstance().subscribe(
		EventType::Debug_TogglePlayerInfo,
		[this](const Event&) {
			showPlayerDebugInfo_ = !showPlayerDebugInfo_;
		});

	camera_ = std::make_unique<Camera>();//状态开始时创建摄像机实例
	camera_->setViewport(0, 0, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT); // 设置摄像机视口为逻辑尺寸，后续增加动态调整视口大小等功能
	camera_->setWorldBounds(0, 0, tileMap_->getMapWidth() * Config::TILE_SIZE, tileMap_->getMapHeight() * Config::TILE_SIZE);
}

PlayingState::~PlayingState() noexcept {
	EventManager::getInstance().unsubscribe(debugSubscriptionId_);
	context_.physicsEngine.unregisterRigidBody(player_->getRigidBody()); // 状态结束时将玩家的刚体从物理引擎中注销
}

void PlayingState::render() const noexcept{
	Renderer::getInstance().clearStaticTexture(); // 切换到静态纹理渲染目标并清除内容，准备渲染地图等静态元素
	tileMap_->render(*camera_);

	Renderer::getInstance().clearDynamicTexture(); // 切换到动态纹理渲染目标并清除内容，准备渲染玩家等动态元素
	player_->render(*camera_);

	if (showPlayerDebugInfo_) {
		player_->renderDebug(*camera_);
	}
}

void PlayingState::update(float dt) noexcept{
	// 顺序： 输入移动 - 重力 - 碰撞修正
	player_->setCommand(Player::PlayerCommand{
		Input::getInstance().getMoveLeftPressed(),
		Input::getInstance().getMoveRightPressed(),
		Input::getInstance().getJumpPressed(),
		Input::getInstance().getAttackPressed(),
		Input::getInstance().getUPPressed(),
		Input::getInstance().getDownPressed(),
		Input::getInstance().getSprintPressed(),
		Input::getInstance().getFlashPressed()
		});
	player_->judgeClimb(tileMap_->isCanClimb(player_->getRigidBody().hitBox)); // 根据当前碰撞盒位置判断是否可以攀爬，并更新玩家的移动模式
	player_->update(dt);

	context_.physicsEngine.update(dt); // 更新物理系统，处理玩家的运动和碰撞

	context_.physicsEngine.resolveCollisions(tileMap_->getPhysicalCollisionMap(), dt, Config::EPSILON); // 处理物理系统中的碰撞检测和响应，根据地图的碰撞数据进行修正

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

	camera_->followTarget(player_->getRigidBody().hitBox, dt, true); // 摄像机跟随玩家，启用平滑跟随

	//test win condition
	if(currentLevel_ == 2) {
		EventManager::getInstance().sendEvent(Event{ EventType::State_Transition, StateType::WON });
		EventManager::getInstance().sendEvent(Event{ EventType::UI_Show, UIType::WON });
	}
}

void PlayingState::loadLevel(const std::string& filePath){
	// 这里可以添加实际的地图加载逻辑，例如从文件中读取地图数据并解析成TileMap对象
	// 目前只是创建一个新的TileMap实例并调用loadFromFile方法作为示例
	tileMap_ = Maploader::loadMap(filePath, Resource::getInstance());
	player_->reset(); // 加载新关卡时重置玩家状态和位置
}
