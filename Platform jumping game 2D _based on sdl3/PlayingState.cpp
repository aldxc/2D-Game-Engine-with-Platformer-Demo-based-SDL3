#include "PlayingState.h"
#include "MapLoader.h"
#include "render/Renderer.h"
#include "input/Input.h"
#include "resource/Resource.h"
#include "core/EventManager.h"
#include "UIType.h"

PlayingState::PlayingState() noexcept : State<StateType>(StateType::PLAYING) {
	currentLevel_ = 0; // 初始关卡编号，后续增加关卡管理等功能

	player_ = std::make_unique<Player>();//状态开始时创建玩家实例，后续增加地图、敌人等元素的创建
	loadLevel(Config::LEVEL_PATH[currentLevel_]); // 加载初始关卡数据，后续增加地图资源管理等功能
	//tileMap_ = Maploader::loadMap("resource/level1.bin", Resource::getInstance()); // 从文件加载地图数据，后续增加地图资源管理等功能

	debugSubscriptionId_ = EventManager::getInstance().subscribe(
		EventType::Debug_TogglePlayerInfo,
		[this](const Event&) {
			showPlayerDebugInfo_ = !showPlayerDebugInfo_;
		});
}

PlayingState::~PlayingState() noexcept {
	EventManager::getInstance().unsubscribe(debugSubscriptionId_);
}

void PlayingState::render() const noexcept{
	Renderer::getInstance().clearStaticTexture(); // 切换到静态纹理渲染目标并清除内容，准备渲染地图等静态元素
	tileMap_->render();

	Renderer::getInstance().clearDynamicTexture(); // 切换到动态纹理渲染目标并清除内容，准备渲染玩家等动态元素
	player_->render();

	if (showPlayerDebugInfo_) {
		player_->renderDebug();
	}
}

void PlayingState::update(float dt) noexcept{
	// 顺序： 输入移动 - 重力 - 碰撞修正
	player_->setCommand(Player::PlayerCommand{
		Input::getInstance().getMoveLeftPressed(),
		Input::getInstance().getMoveRightPressed(),
		Input::getInstance().getJumpPressed()
		});
	player_->update(dt);
	//if(!player_->isLanded()) player_->applyGravity(500.0f, dt); 
	player_->applyGravity(Config::GRAVITY, dt);
	// 碰撞检测和修正
	const TileMap::CollisionResult result = tileMap_->tileCollision(player_->getHitBox(), player_->getVelocityX(), player_->getVelocityY(), player_->isLanded(), dt);
	player_->applyResolvedMovement(result.hitbox, result.velocityX, result.velocityY, result.isLanded);

	player_->updateAnimationState(dt);

	//test 切换关卡
	if (player_->getHitBox().x > 800) {
		loadLevel(Config::LEVEL_PATH[++currentLevel_]);
	}
	//test win condition
	if(currentLevel_ == 2) {
		EventManager::getInstance().sendEvent(Event{ EventType::State_Transition, StateType::WON });
		EventManager::getInstance().sendEvent(Event{ EventType::UI_Show, UIType::WON });
	}
}

void PlayingState::loadLevel(const std::string& filePath){
	// 这里可以添加实际的地图加载逻辑，例如从文件中读取地图数据并解析成TileMap对象
	// 目前只是创建一个新的TileMap实例并调用loadFromFile方法作为示例
	tileMap_ = std::make_unique<TileMap>();
	tileMap_ = Maploader::loadMap(filePath, Resource::getInstance());
	player_->reset(); // 加载新关卡时重置玩家状态和位置
}
