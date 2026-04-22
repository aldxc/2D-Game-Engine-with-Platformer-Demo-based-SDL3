#include <string>
#include <algorithm>
#include "Enemy.h"
#include "resource/Resource.h"
#include "render/Renderer.h"
#include "core/Rect.h"
#include "render/Camera.h"
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

void Enemy::update(double dt) noexcept {
	// AI移动逻辑，只根据face移动，face通过地图碰撞信息玩家等调整
	int dir = isFacingRight_ ? 1 : -1;
	if(isAlreadyTracking_ || !rigidBody_.isLanded) {
		dir = 0; // 已经跟踪到玩家位置，停止移动
	}
	if (!isHited_ && !hitInvincibleTimer_.isActive() && isAlive_) {
		rigidBody_.velocity.setX(dir * Config::ENEMY_MAX_SPEED);
	}

	isHited_ = hitInvincibleTimer_.isActive(); // 根据受击无敌计时器状态更新受伤状态
	deathTimer_.update(dt);
	trackingTimer_.update(dt);
	hitInvincibleTimer_.update(dt);

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

	if (!isAlive_) {
		// deathTimer_ 还在倒计时，剩余时间 / 总时间 = 剩余 alpha 比例
		float remaining = 1.0f - deathTimer_.getElapsedTime() / deathTimer_.getDurationTime();
		SDL_SetTextureAlphaModFloat(enemyTexture_.get(), remaining);
	}

	// 显示血条
	Rect currentHpRect = Rect{ rigidBody_.hitBox.x(), rigidBody_.hitBox.y() - 20, rigidBody_.hitBox.w() * (hp_ / static_cast<float>(maxHp_)), Config::BLOOD_RECT_HEIGHT };
	Rect healthBarRect = Rect{ rigidBody_.hitBox.x(), rigidBody_.hitBox.y() - 20, rigidBody_.hitBox.w(), Config::BLOOD_RECT_HEIGHT };

	renderer_.renderFillRect(camera.worldToScreen(currentHpRect), SDL_Color({ 255, 0, 0, 255 })); // 红色血条
	renderer_.renderRect(camera.worldToScreen(healthBarRect), SDL_Color({ 255, 255, 255, 255 })); // 白色血条边框

	if (isFacingRight_) {
		renderer_.renderTexture(enemyTexture_.get(), currentFrameRect, scaleTextureRect);
	}
	else {
		renderer_.reversePlayerFaceTexture(enemyTexture_.get(), currentFrameRect, scaleTextureRect);
	}

	if (!isAlive_) {
		SDL_SetTextureAlphaMod(enemyTexture_.get(), 255);
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
	hp_ = Config::ENEMY_HP;
	Object::reset();
}

void Enemy::setFacingRight(const Rect& player, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept{
	if (!rigidBody_.isLanded || !isAlive_) {
		return;
	}

	const bool playerOnRight = (player.x() > rigidBody_.hitBox.x());
	const bool desiredFacingRight = playerOnRight;

	const float dis = rigidBody_.hitBox.distance(player);
	const bool tooFar = dis >= Config::ENEMY_SCAN_DISTANCE;
	const bool blockedByWall = !Physics::LineOfSight(rigidBody_.hitBox, player, collmap);

	const bool canTrack = !trackingTimer_.isActive() && !tooFar && !blockedByWall;

	if (!canTrack) {
		isTrackingPlayer_ = false;
		isAlreadyTracking_ = false;

		const bool wallAhead = Physics::hasWallAhead(rigidBody_.hitBox, isFacingRight_, collmap);
		const bool cliffAhead = !Physics::hasGroundAhead(rigidBody_.hitBox, isFacingRight_, collmap);

		if (wallAhead || cliffAhead) {
			isFacingRight_ = !isFacingRight_;
		}
		return;
	}

	const bool wallAheadDesired = Physics::hasWallAhead(rigidBody_.hitBox, desiredFacingRight, collmap);
	const bool cliffAheadDesired = !Physics::hasGroundAhead(rigidBody_.hitBox, desiredFacingRight, collmap);

	if (cliffAheadDesired) {
		isFacingRight_ = !desiredFacingRight;
		isTrackingPlayer_ = false;
		isAlreadyTracking_ = false;
		trackingTimer_.start(Config::ENEMY_TRACKING_COOLDOWN);
		return;
	}

	if (wallAheadDesired) {
		isFacingRight_ = !desiredFacingRight;
		isTrackingPlayer_ = false;
		isAlreadyTracking_ = false;
		return;
	}

	isTrackingPlayer_ = true;
	isFacingRight_ = desiredFacingRight;
	isAlreadyTracking_ = std::abs(player.x() - rigidBody_.hitBox.x()) < 2.0f;
}

void Enemy::updateAnimationState(double dt) noexcept {
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
	deathTimer_.start(Config::ENEMY_DEAD_DURATION); // 启动死亡定时器，设置持续时间为1秒，后续可以根据需要调整时间
}

void Enemy::takeHit(int dir) noexcept{
	if (!hitInvincibleTimer_.isActive()) {
		rigidBody_.velocity.setX(dir * Config::ENEMY_HITBACK_VELOCITY);
		sfxToplay_.push_back(SfxId::PlayerAttackHit);
		if (--hp_ <= 0) {
			kill();
			return;
		}
		// 根据攻击方向调整敌人受伤后的反应，例如被击退等，后续增加更多的受伤逻辑如眩晕、状态变化等
		hitInvincibleTimer_.start(Config::ENEMY_HIT_INVINCIBILITY_DURATION); // 启动受击无敌计时器，设置持续时间为0.5秒，后续可以根据需要调整时间
		isHited_ = true;
	}
}

