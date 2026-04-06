#include "PlayingState.h"
#include "MapLoader.h"
#include "render/Renderer.h"
#include "input/Input.h"
#include "resource/Resource.h"
#include "render/Animation.h"

PlayingState::PlayingState(Animation& animation) noexcept : State<StateType>(StateType::PLAYING) {
	currentLevel_ = 0; // 初始关卡编号，后续增加关卡管理等功能

	player_ = std::make_unique<Player>(animation);//状态开始时创建玩家实例，后续增加地图、敌人等元素的创建
	loadLevel(Config::LEVEL_PATH[currentLevel_]); // 加载初始关卡数据，后续增加地图资源管理等功能
	//tileMap_ = Maploader::loadMap("resource/level1.bin", Resource::getInstance()); // 从文件加载地图数据，后续增加地图资源管理等功能
}

void PlayingState::render() const noexcept{
	tileMap_->render();
	player_->render();
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
	player_->applyGravity(500.0f, dt);
	// 碰撞检测和修正
	const TileMap::CollisionResult result = tileMap_->tileCollision(player_->getHitBox(), player_->getVelocityX(), player_->getVelocityY(), player_->isLanded(), dt);
	player_->applyResolvedMovement(result.hitbox, result.velocityX, result.velocityY, result.isLanded);

	player_->updateAnimationState(dt);

	//test 切换关卡
	if (player_->getHitBox().x > 800) {
		loadLevel(Config::LEVEL_PATH[++currentLevel_]);
	}
}

void PlayingState::loadLevel(const std::string& filePath){
	// 这里可以添加实际的地图加载逻辑，例如从文件中读取地图数据并解析成TileMap对象
	// 目前只是创建一个新的TileMap实例并调用loadFromFile方法作为示例
	tileMap_ = std::make_unique<TileMap>();
	tileMap_ = Maploader::loadMap(filePath, Resource::getInstance());
	player_->reset(); // 加载新关卡时重置玩家状态和位置
}
