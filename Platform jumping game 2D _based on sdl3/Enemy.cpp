#include <string>
#include "Enemy.h"
#include "resource/Resource.h"
#include "render/Renderer.h"
#include "core/Rect.h"
#include "render/Camera.h"
//#include "physics/PhysicsCollMap.h"
#include "physics/Physics.h"

Enemy::Enemy(Renderer& renderer, Resource& rM) noexcept : rigidBody_(Vec2(0, 0), Rect{ 0,0,0,0 } , Config::MAX_SPEED), renderer_(renderer) {
	// 这里可以添加敌人的初始化逻辑，例如设置初始位置、加载纹理等
	enemyTexture_ = rM.loadTexture("resource/characters.png", renderer_.getSDLRenderer());

	reset();
}

void Enemy::init(Rect enemyInfo) noexcept{
	// 这里可以添加敌人状态初始化的逻辑，例如设置位置、动画状态等，后续增加更多的初始化功能如AI状态等
	rigidBody_.velocity = Vec2(0, 0);
	rigidBody_.hitBox = enemyInfo;
	rigidBody_.isLanded = false;
	rigidBody_.acceleration = Vec2(0, 0);
	rigidBody_.gravityScale = 1.0f;
	rigidBody_.maxSpeed = Config::ENEMY_MAX_SPEED;
	rigidBody_.maxFallSpeed = Config::ENEMY_MAX_FALL_SPEED;
	currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation::AnimationClip animationClip;
	const auto src = Config::ENEMY_IDLE_SRC;
	animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
	animation_.play(animationClip);
	isAlive_ = true;
	isFacingRight_ = true;      
	isTrackingPlayer_ = false;  
	isAlreadyTracking_ = false;
	isRegisteredToPhysics_ = false; 
}

void Enemy::update(float dt) noexcept {
	// AI移动逻辑，只根据face移动，face通过地图碰撞信息玩家等调整
	int dir = isFacingRight_ ? 1 : -1;
	if(isAlreadyTracking_) {
		dir = 0; // 已经跟踪到玩家位置，停止移动
	}
	rigidBody_.velocity.setX(dir * Config::ENEMY_MAX_SPEED);

	deathTimer_.update(dt);
	trackingTimer_.update(dt);

	if(!isAlive_ && !deathTimer_.isActive()) {
		// 死亡定时器结束，敌人可以被重置或销毁
		setIsDestroyed(true); // 使敌人失活，等待重置或销毁
		deathTimer_.stop(); // 停止定时器，重置状态
	}
}

void Enemy::render(const Camera& camera) const noexcept {
	// 这里可以添加敌人的渲染逻辑，例如根据状态选择不同的动画帧等
	Rect scaleTextureRect = rigidBody_.hitBox;
	scaleTextureRect.setW(scaleTextureRect.w() * 2);
	scaleTextureRect.setH(scaleTextureRect.h() * 2);
	scaleTextureRect.setX(scaleTextureRect.x() - 16);
	scaleTextureRect.setY(scaleTextureRect.y() - 32);
	scaleTextureRect = camera.worldToScreen(scaleTextureRect); // 将敌人的世界坐标转换为屏幕坐标

	Rect currentFrameRect = animation_.getCurrentFrameRect();
	//renderer_.renderTexture(enemyTexture_.get(), currentFrameRect, scaleTextureRect);
	if (isFacingRight_) {
		renderer_.renderTexture(enemyTexture_.get(), currentFrameRect, scaleTextureRect);
	}
	else {
		renderer_.reversePlayerFaceTexture(enemyTexture_.get(), currentFrameRect, scaleTextureRect);
	}

}

void Enemy::renderDebug(const Camera& camera) const noexcept {
	SDL_Color debugTextColor = SDL_Color({ 255, 255, 255, 255 }); // 白色文本
	SDL_FRect debugInfoRect = { 0, 0, 200, 200 };

	// 这里可以添加敌人的调试渲染逻辑，例如渲染AI状态、路径等
	renderer_.renderRect(camera.worldToScreen(rigidBody_.hitBox), SDL_Color({ 255, 0, 0, 255 })); // 用红色矩形表示敌人碰撞盒
	std::string enemyStateText = "Facing: " + std::string(isFacingRight_ ? "Right" : "Left");
	renderer_.renderText(enemyStateText, debugInfoRect, debugTextColor, 20);
	enemyStateText = " isAlready : " + std::string(isAlreadyTracking_ ? "Yes" : "No");
	renderer_.renderText(enemyStateText, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 30, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	enemyStateText = " isAlive : " + std::string(isAlive_ ? "Yes" : "No");
	renderer_.renderText(enemyStateText, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 60, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	enemyStateText = "isLanded : " + std::string(rigidBody_.isLanded ? "Yes" : "No");
	renderer_.renderText(enemyStateText, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 90, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	enemyStateText = "isTracking : " + std::string(isTrackingPlayer_ ? "Yes" : "No");
	renderer_.renderText(enemyStateText, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 120, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
}

void Enemy::reset(Rect hitbox) noexcept {
	// 这里可以添加敌人状态重置的逻辑，例如重置位置、动画状态等
	rigidBody_.velocity = Vec2(0, 0);
	rigidBody_.hitBox = hitbox;
	rigidBody_.isLanded = false;
	rigidBody_.acceleration = Vec2(0, 0);
	rigidBody_.gravityScale = 1.0f;
	rigidBody_.maxSpeed = Config::ENEMY_MAX_SPEED;
	rigidBody_.maxFallSpeed = Config::ENEMY_MAX_FALL_SPEED;
	currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation::AnimationClip animationClip;
	const auto src = Config::ENEMY_IDLE_SRC;
	animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
	animation_.play(animationClip);

	isAlive_ = true;            
	isFacingRight_ = true;      
	isTrackingPlayer_ = false;  
	isAlreadyTracking_ = false;  
	isRegisteredToPhysics_ = false;
	Object::reset();
}

void Enemy::setFacingRight(const Rect& player, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept{
	// 跟踪：与玩家间有full时不跟踪，距离过远时不跟踪，其他情况（前方是悬崖也不跟踪）
	bool playerOnRight = (player.x() > rigidBody_.hitBox.x());
	// 不跟踪：检查敌人和玩家之间是否有完全碰撞，同时检查敌人和玩家之间的距离，前方为悬崖时不跟踪（启用计时器防止频繁回头）
	float dis = rigidBody_.hitBox.distance(player);
	bool f1 = dis >= Config::ENEMY_SCAN_DISTANCE;
	bool f2 = !Physics::LineOfSight(rigidBody_.hitBox, player, collmap);
	bool f3 = false;
	if (playerOnRight) {
		// 检查敌人右侧是否有地面
		Rect rightCheckBox = rigidBody_.hitBox;
		rightCheckBox.setX(rightCheckBox.x() + rightCheckBox.w() + 1); // 在敌人右侧稍微偏移一点
		rightCheckBox.setY(rightCheckBox.y() + rightCheckBox.h() + 1); // 在敌人下方稍微偏移一点
		f3 = !Physics::LineOfSight(rightCheckBox, rightCheckBox, collmap); // 如果右侧没有地面，则视为悬崖
		isFacingRight_ = !f3;
	} else {
		// 检查敌人左侧是否有地面
		Rect leftCheckBox = rigidBody_.hitBox;
		leftCheckBox.setX(leftCheckBox.x() - 1); // 在敌人左侧稍微偏移一点
		leftCheckBox.setY(leftCheckBox.y() + leftCheckBox.h() + 1); // 在敌人下方稍微偏移一点
		f3 = !Physics::LineOfSight(leftCheckBox, leftCheckBox, collmap); // 如果左侧没有地面，则视为悬崖
		isFacingRight_ = f3; // 如果左侧没有地面，敌人应该朝右，否则朝左
	}
	if (f3) {
		trackingTimer_.start(Config::ENEMY_TRACKING_COOLDOWN); // 启动悬崖检测计时器，设置持续时间为0.5秒，后续可以根据需要调整时间
	}
	if (f1 || f2 || f3 || trackingTimer_.isActive()) { // 满足任一条件则不跟踪
		isTrackingPlayer_ = false;
	} else {
		isTrackingPlayer_ = true;
	}

	if(isTrackingPlayer_ && !trackingTimer_.isActive()) {
		if (playerOnRight) {
			// 玩家在右边，敌人朝右
			isFacingRight_ = true;
		}
		else {
			// 玩家在左边，敌人朝左
			isFacingRight_ = false;
		}
		if (std::abs(player.x() - rigidBody_.hitBox.x()) < 2.0f) {
			isAlreadyTracking_ = true;
		}
		else {
			isAlreadyTracking_ = false;
		}
	}
}

void Enemy::updateAnimationState(float dt) noexcept {
	// 这里可以添加根据敌人状态更新动画状态的逻辑，例如根据移动状态切换动画剪辑等
	if (isStateChanged()) {
		Animation::AnimationClip animationClip;
		switch (currentAnimationState_) {
			case PlayerAnimationState::IDLE: {
				const auto src = Config::ENEMY_IDLE_SRC;
				animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
				break;
			}
			case PlayerAnimationState::RUN: {
				const auto src = Config::ENEMY_RUN_SRC;
				for (const auto& rect : src) {
					animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
				}
				break;
			}
		}
		animation_.play(animationClip);
	}
	animation_.update(dt);
}

bool Enemy::isStateChanged() noexcept {
	PlayerAnimationState nextState = PlayerAnimationState::IDLE;
	if(rigidBody_.velocity.getX() > 0.1f || rigidBody_.velocity.getX() < -0.1f) {
		nextState = PlayerAnimationState::RUN;
	}
	bool changed = (nextState != currentAnimationState_);
	currentAnimationState_ = nextState;
	return changed;
}

void Enemy::kill() noexcept {
	isAlive_ = false;
	deathTimer_.start(2.0f); // 启动死亡定时器，设置持续时间为1秒，后续可以根据需要调整时间
}