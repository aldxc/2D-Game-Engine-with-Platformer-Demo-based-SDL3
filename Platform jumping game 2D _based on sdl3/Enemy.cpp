#include <string>
#include <algorithm>
#include "Enemy.h"
#include "resource/Resource.h"
#include "render/Renderer.h"
#include "core/Rect.h"
#include "render/Camera.h"
#include "physics/Physics.h"

Enemy::Enemy(Renderer& renderer, Resource& rM) noexcept : m_rigidBody(Vec2(0, 0), Rect{ 0,0,0,0 } , Config::MAX_SPEED), m_renderer(renderer) {
	// 这里可以添加敌人的初始化逻辑，例如设置初始位置、加载纹理等
	m_enemyTexture = rM.loadTexture(Config::ENEMY_TEXTURE_PATH, m_renderer.getSDLRenderer());

	reset();
}

void Enemy::init(Rect enemyInfo) noexcept{
	m_rigidBody.velocity = Vec2(0, 0);
	m_rigidBody.hitBox = enemyInfo;
	m_rigidBody.isLanded = false;
	m_rigidBody.acceleration = Vec2(0, 0);
	m_rigidBody.gravityScale = 1.0f;
	m_rigidBody.maxSpeed = Config::ENEMY_MAX_SPEED;
	m_rigidBody.maxFallSpeed = Config::ENEMY_MAX_FALL_SPEED;
	m_currentAnimationState = PlayerAnimationState::IDLE;
	Animation::AnimationClip animationClip;
	const auto src = Config::ENEMY_IDLE_SRC;
	animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
	m_animation.play(animationClip);
	m_isAlive = true;
	m_isFacingRight = true;      
	m_isTrackingPlayer = false;  
	m_isAlreadyTracking = false;
	m_isRegisteredToPhysics = false; 
}

void Enemy::update(double dt) noexcept {
	// AI移动逻辑，只根据face移动，face通过地图碰撞信息玩家等调整
	int dir = m_isFacingRight ? 1 : -1;
	if(m_isAlreadyTracking || !m_rigidBody.isLanded) {
		// 已经跟踪到玩家位置，停止移动
		dir = 0; 
	}
	if (!m_isHited && !m_hitInvincibleTimer.isActive() && m_isAlive) {
		m_rigidBody.velocity.setX(dir * Config::ENEMY_MAX_SPEED);
	}

	m_isHited = m_hitInvincibleTimer.isActive(); 
	m_deathTimer.update(dt);
	m_trackingTimer.update(dt);
	m_hitInvincibleTimer.update(dt);

	if(!m_isAlive && !m_deathTimer.isActive()) {
		// 死亡定时器结束，敌人可以被重置或销毁
		setIsDestroyed(true); 
		m_deathTimer.stop(); 
	}
}

void Enemy::render(const Camera& camera) const noexcept {
	// 这里可以添加敌人的渲染逻辑，例如根据状态选择不同的动画帧等
	Rect scaleTextureRect = m_rigidBody.hitBox;
	scaleTextureRect.setW(scaleTextureRect.w() * 2);
	scaleTextureRect.setH(scaleTextureRect.h() * 2);
	scaleTextureRect.setX(scaleTextureRect.x() - 16);
	scaleTextureRect.setY(scaleTextureRect.y() - 32);
	scaleTextureRect = camera.worldToScreen(scaleTextureRect); 

	Rect currentFrameRect = m_animation.getCurrentFrameRect();

	if (!m_isAlive) {
		// m_deathTimer 还在倒计时，剩余时间 / 总时间 = 剩余 alpha 比例
		float remaining = 1.0f - m_deathTimer.getElapsedTime() / m_deathTimer.getDurationTime();
		SDL_SetTextureAlphaModFloat(m_enemyTexture.get(), remaining);
	}

	// 显示血条
	Rect currentHpRect = Rect{ m_rigidBody.hitBox.x(), m_rigidBody.hitBox.y() - 20, m_rigidBody.hitBox.w() * (m_hp / static_cast<float>(m_maxHp)), Config::BLOOD_RECT_HEIGHT };
	Rect healthBarRect = Rect{ m_rigidBody.hitBox.x(), m_rigidBody.hitBox.y() - 20, m_rigidBody.hitBox.w(), Config::BLOOD_RECT_HEIGHT };

	m_renderer.renderFillRect(camera.worldToScreen(currentHpRect), SDL_Color({ 255, 0, 0, 255 })); 
	m_renderer.renderRect(camera.worldToScreen(healthBarRect), SDL_Color({ 255, 255, 255, 255 })); 

	if (m_isFacingRight) {
		m_renderer.renderTexture(m_enemyTexture.get(), currentFrameRect, scaleTextureRect);
	}
	else {
		m_renderer.reversePlayerFaceTexture(m_enemyTexture.get(), currentFrameRect, scaleTextureRect);
	}

	if (!m_isAlive) {
		SDL_SetTextureAlphaMod(m_enemyTexture.get(), 255);
	}
}

void Enemy::renderDebug(const Camera& camera) const noexcept {
	SDL_Color debugTextColor = SDL_Color({ 255, 255, 255, 255 }); // 白色文本
	SDL_FRect debugInfoRect = { 0, 0, 200, 200 };

	// 这里可以添加敌人的调试渲染逻辑，例如渲染AI状态、路径等
	m_renderer.renderRect(camera.worldToScreen(m_rigidBody.hitBox), SDL_Color({ 255, 0, 0, 255 })); // 用红色矩形表示敌人碰撞盒
	std::string enemyStateText = "Facing: " + std::string(m_isFacingRight ? "Right" : "Left");
	m_renderer.renderText(enemyStateText, debugInfoRect, debugTextColor, 20);
	enemyStateText = " isAlready : " + std::string(m_isAlreadyTracking ? "Yes" : "No");
	m_renderer.renderText(enemyStateText, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 30, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	enemyStateText = " isAlive : " + std::string(m_isAlive ? "Yes" : "No");
	m_renderer.renderText(enemyStateText, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 60, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	enemyStateText = "isLanded : " + std::string(m_rigidBody.isLanded ? "Yes" : "No");
	m_renderer.renderText(enemyStateText, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 90, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	enemyStateText = "isTracking : " + std::string(m_isTrackingPlayer ? "Yes" : "No");
	m_renderer.renderText(enemyStateText, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 120, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
}

void Enemy::reset(Rect hitbox) noexcept {
	m_rigidBody.velocity = Vec2(0, 0);
	m_rigidBody.hitBox = hitbox;
	m_rigidBody.isLanded = false;
	m_rigidBody.acceleration = Vec2(0, 0);
	m_rigidBody.gravityScale = 1.0f;
	m_rigidBody.maxSpeed = Config::ENEMY_MAX_SPEED;
	m_rigidBody.maxFallSpeed = Config::ENEMY_MAX_FALL_SPEED;
	m_currentAnimationState = PlayerAnimationState::IDLE;
	Animation::AnimationClip animationClip;
	const auto src = Config::ENEMY_IDLE_SRC;
	animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
	m_animation.play(animationClip);

	m_isAlive = true;            
	m_isFacingRight = true;      
	m_isTrackingPlayer = false;  
	m_isAlreadyTracking = false;  
	m_isRegisteredToPhysics = false;
	m_hp = Config::ENEMY_HP;
	Object::reset();
}

void Enemy::setFacingRight(const Rect& player, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept{
	if (!m_rigidBody.isLanded || !m_isAlive) {
		return;
	}

	const bool playerOnRight = (player.x() > m_rigidBody.hitBox.x());
	const bool desiredFacingRight = playerOnRight;

	const float dis = m_rigidBody.hitBox.distance(player);
	const bool tooFar = dis >= Config::ENEMY_SCAN_DISTANCE;
	const bool blockedByWall = !Physics::LineOfSight(m_rigidBody.hitBox, player, collmap);

	const bool canTrack = !m_trackingTimer.isActive() && !tooFar && !blockedByWall;

	if (!canTrack) {
		m_isTrackingPlayer = false;
		m_isAlreadyTracking = false;

		const bool wallAhead = Physics::hasWallAhead(m_rigidBody.hitBox, m_isFacingRight, collmap);
		const bool cliffAhead = !Physics::hasGroundAhead(m_rigidBody.hitBox, m_isFacingRight, collmap);

		if (wallAhead || cliffAhead) {
			m_isFacingRight = !m_isFacingRight;
		}
		return;
	}

	const bool wallAheadDesired = Physics::hasWallAhead(m_rigidBody.hitBox, desiredFacingRight, collmap);
	const bool cliffAheadDesired = !Physics::hasGroundAhead(m_rigidBody.hitBox, desiredFacingRight, collmap);

	if (cliffAheadDesired) {
		m_isFacingRight = !desiredFacingRight;
		m_isTrackingPlayer = false;
		m_isAlreadyTracking = false;
		m_trackingTimer.start(Config::ENEMY_TRACKING_COOLDOWN);
		return;
	}

	if (wallAheadDesired) {
		m_isFacingRight = !desiredFacingRight;
		m_isTrackingPlayer = false;
		m_isAlreadyTracking = false;
		return;
	}

	m_isTrackingPlayer = true;
	m_isFacingRight = desiredFacingRight;
	m_isAlreadyTracking = std::abs(player.x() - m_rigidBody.hitBox.x()) < 2.0f;
}

void Enemy::updateAnimationState(double dt) noexcept {
	if (isStateChanged()) {
		Animation::AnimationClip animationClip;
		switch (m_currentAnimationState) {
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
		m_animation.play(animationClip);
	}
	m_animation.update(dt);
}

bool Enemy::isStateChanged() noexcept {
	PlayerAnimationState nextState = PlayerAnimationState::IDLE;
	if(m_rigidBody.velocity.x() > 0.1f || m_rigidBody.velocity.x() < -0.1f) {
		nextState = PlayerAnimationState::RUN;
	}
	bool changed = (nextState != m_currentAnimationState);
	m_currentAnimationState = nextState;
	return changed;
}

void Enemy::kill() noexcept {
	m_isAlive = false;
	// 启动死亡定时器
	m_deathTimer.start(Config::ENEMY_DEAD_DURATION); 
}

void Enemy::takeHit(int dir) noexcept{
	if (!m_hitInvincibleTimer.isActive()) {
		m_rigidBody.velocity.setX(dir * Config::ENEMY_HITBACK_VELOCITY);
		m_sfxToplay.push_back(SfxId::PLAYER_ATTACK_HIT);
		if (--m_hp <= 0) {
			kill();
			return;
		}
		// 启动受击无敌计时器
		m_hitInvincibleTimer.start(Config::ENEMY_HIT_INVINCIBILITY_DURATION); 
		m_isHited = true;
	}
}

