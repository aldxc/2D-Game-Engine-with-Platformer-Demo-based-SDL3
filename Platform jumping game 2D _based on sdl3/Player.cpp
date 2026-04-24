#include <algorithm>
#include <cmath>
#include <array>
#include "Player.h"
#include "render/Renderer.h"
#include "render/Camera.h"
#include "resource/Resource.h"
#include "physics/Physics.h"
#include "core/Rect.h"

Player::Player(Renderer& renderer, Resource& rM) noexcept : m_renderer(renderer){
	m_playerTexture = rM.loadTexture(Config::PLAYER_TEXTURE_PATH, m_renderer.getSDLRenderer());
	m_attackTexture = rM.loadTexture(Config::ATTACK_TEXTURE_PATH, m_renderer.getSDLRenderer());
	m_command = {};

	m_currentAnimationState = PlayerAnimationState::IDLE;
	Animation::AnimationClip animationClip;
	const auto src = Config::PLAYER_IDLE_SRC;
	animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
	m_animation.play(animationClip);

	m_attackDuration = Config::ATTACK_DURATION;
	m_attackHitBox = { 0,0,Config::ATTACK_WIDTH, Config::ATTACK_HEIGHT };
	m_sprintDirection = Vec2(0, 0);
	m_sprintMaxCount = Config::SPRINT_MAX_COUNT;
	m_sprintCount = 0;
	m_birthPoint = Vec2(0, 0);
	m_rigidBody = RigidBody(Vec2(0, 0), Rect(m_birthPoint.x(), m_birthPoint.y(), Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT), Config::MAX_SPEED);
}

void Player::update(double dt) noexcept{
	updatePublicStatus(dt);

	updateMovementAcceleration(dt);

	finalizeState();
}

void Player::render(const Camera& camera) const noexcept{
	Rect scaleTextureRect = m_rigidBody.hitBox;
	// 为了让玩家在屏幕上看起来更大一些，这里对渲染的矩形进行缩放
	scaleTextureRect.setW(scaleTextureRect.w() * 2);
	scaleTextureRect.setH(scaleTextureRect.h() * 2);
	scaleTextureRect.setX(scaleTextureRect.x() - 16);
	scaleTextureRect.setY(scaleTextureRect.y() - 32);

	std::array<float, 3> offsets = { Config::PLAYER_1_Y_OFFSET, Config::PLAYER_2_Y_OFFSET, Config::PLAYER_3_Y_OFFSET };

	Rect currentFrameRect = m_animation.getCurrentFrameRect();
	currentFrameRect.setY(currentFrameRect.y() + offsets[m_playerId]); 

	scaleTextureRect = camera.worldToScreen(scaleTextureRect);

	// 显示血条
	Rect currentHpRect = Rect{ m_rigidBody.hitBox.x(), m_rigidBody.hitBox.y() - 20, m_rigidBody.hitBox.w() * (m_hp / static_cast<float>(m_maxHp)), Config::BLOOD_RECT_HEIGHT };
	Rect healthBarRect = Rect{ m_rigidBody.hitBox.x(), m_rigidBody.hitBox.y() - 20, m_rigidBody.hitBox.w(), Config::BLOOD_RECT_HEIGHT };

	m_renderer.renderFillRect(camera.worldToScreen(currentHpRect), SDL_Color({ 255, 0, 0, 255 })); 
	m_renderer.renderRect(camera.worldToScreen(healthBarRect), SDL_Color({ 255, 255, 255, 255 }));

	// 受击时闪烁效果，后续可以根据需要调整闪烁频率、持续时间等参数
	// 这里使用正弦函数来实现闪烁效果，alpha值在0.3到1.0之间变化，频率为10Hz
	if (m_isHited && m_hitTimer.isActive()) {
		float progress = m_hitTimer.getElapsedTime();
		float alpha = 0.3f + 0.7f * (0.5f + 0.5f * std::sin(progress * 10.0f));
		SDL_SetTextureAlphaModFloat(m_playerTexture.get(), alpha);
	}

	if (m_facingRight) {
		m_renderer.renderTexture(m_playerTexture.get(), currentFrameRect, scaleTextureRect);
	}
	else {
		m_renderer.reversePlayerFaceTexture(m_playerTexture.get(), currentFrameRect, scaleTextureRect);
	}

	// 恢复alpha，防止影响其他使用同一纹理的对象
	if (m_isHited && m_hitTimer.isActive()) {
		SDL_SetTextureAlphaModFloat(m_playerTexture.get(), 1.0f);
	}

	if (m_isAttacking) {
		const auto& src = Config::ATTACK_SRC[m_animation.getCurrentFrameIndex()];
		Rect attackSrcRect = Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) };
		Rect attackDstRect = camera.worldToScreen(m_attackHitBox);

		if (m_attackFacingRight) {
			m_renderer.renderTexture(m_attackTexture.get(), attackSrcRect, attackDstRect);
		}
		else {
			m_renderer.reversePlayerFaceTexture(m_attackTexture.get(), attackSrcRect, attackDstRect);
		}
	}
}

void Player::renderDebug(const Camera& camera) const noexcept{
	SDL_Color debugTextColor = SDL_Color({ 255, 255, 255, 255 }); // 白色文本
	SDL_FRect debugInfoRect = { Config::LOGIC_WIDTH - 200, 0, 200, 200 };
	std::string str = (m_rigidBody.isLanded) ? "Landed" : "Air";
	m_renderer.renderText("IsLand:" + str, debugInfoRect, debugTextColor, 20);
	std::string velocityXStr = "VelX: " + std::to_string(static_cast<int>(m_rigidBody.velocity.x()));
	std::string velocityYStr = "VelY: " + std::to_string(static_cast<int>(m_rigidBody.velocity.y()));
	m_renderer.renderText(velocityXStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 30, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	m_renderer.renderText(velocityYStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 60, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string currState = "State: ";
	switch (m_currentAnimationState) {
	case PlayerAnimationState::IDLE: currState += "Idle"; break;
	case PlayerAnimationState::RUN: currState += "Run"; break;
	case PlayerAnimationState::JUMP: currState += "Jump"; break; 
	case PlayerAnimationState::FALL: currState += "Fall"; break;
	case PlayerAnimationState::ATTACK: currState += "Attack"; break;
	case PlayerAnimationState::CLIMB: currState += "Climb"; break;
	case PlayerAnimationState::SPRINT: currState += "Sprint"; break;
	case PlayerAnimationState::HIT: currState += "Hited"; break;
	}
	m_renderer.renderText(currState, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 90, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string playerPosStr = "Pos: (" + std::to_string(static_cast<int>(m_rigidBody.hitBox.x())) + ", " + std::to_string(static_cast<int>(m_rigidBody.hitBox.y())) + ")";
	m_renderer.renderText(playerPosStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 120, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string cameraMoveStr = "CameraMove: (" + std::to_string(static_cast<int>(camera.getViewport().x())) + ", " + std::to_string(static_cast<int>(camera.getViewport().y())) + ")";
	m_renderer.renderText(cameraMoveStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 150, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string moveModeStr = "MoveMode: " + std::string((m_moveMode == MoveMode::NORMAL) ? "Normal" : (m_moveMode == MoveMode::CLIMB) ? "Climb" : "Sprint");
	m_renderer.renderText(moveModeStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 180, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string hit = "is hited: " + std::string((m_isHited) ? "true" : "false");
	m_renderer.renderText(hit, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 210, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	m_renderer.renderRect(camera.worldToScreen(m_rigidBody.hitBox), SDL_Color({255, 0, 0, 255})); // 用红色矩形表示玩家碰撞盒
	m_renderer.renderRect(camera.worldToScreen(m_attackHitBox), (m_isAttacking ? m_renderer.setColorAlpha(255, 0, 0, 255) : m_renderer.setColorAlpha(200, 200, 200, 255)));
}

void Player::reset() noexcept {
	m_rigidBody.velocity = Vec2(0, 0);
	m_rigidBody.hitBox = Rect(m_birthPoint.x(), m_birthPoint.y(), Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT);
	m_rigidBody.isLanded = false;
	m_rigidBody.acceleration = Vec2(0, 0);
	m_rigidBody.gravityScale = 1.0f;
	m_rigidBody.maxSpeed = Config::MAX_SPEED;

	m_facingRight = true;
	m_isJumping = false;
	m_jumpRequested = false;
	m_isAttacking = false;
	m_isSprinting = false;
	m_wasLanded = false;
	m_isHited = false;
	m_isDead = false;
	m_sprintDirection = Vec2(0, 0);
	m_sprintCount = 0;
	m_hp = Config::PLAYER_HP;
	m_isWinner = false;
}

bool Player::isStateChanged() noexcept{
	PlayerAnimationState nextState = PlayerAnimationState::IDLE;
	if (m_rigidBody.isLanded) {
		if (m_rigidBody.velocity.x() <= 0.1f && m_rigidBody.velocity.x() >= -0.1f) {
			nextState = PlayerAnimationState::IDLE;
		}
		else 
			nextState = PlayerAnimationState::RUN;
	}
	else {
		if (m_rigidBody.velocity.y() < -0.1f) {
			nextState = PlayerAnimationState::JUMP;
		}
		else if (m_rigidBody.velocity.y() > 0.1f) {
			nextState = PlayerAnimationState::FALL;
		}
		else {
			nextState = PlayerAnimationState::FALL;
		}
	}
	if (m_isSprinting) {
		nextState = PlayerAnimationState::SPRINT;
	}
	if (m_isAttacking) {
		nextState = PlayerAnimationState::ATTACK;
	}
	if(m_isHited && !m_hitTimer.isActive()) {
		nextState = PlayerAnimationState::HIT;
	}
	if(m_isWinner) {
		nextState = PlayerAnimationState::WIN;
	}
	bool f = (nextState != m_currentAnimationState);
	m_currentAnimationState = nextState;
	return f;
}

void Player::updateAnimationState(double dt) noexcept{
	if (isStateChanged()) {
		Animation::AnimationClip animationClip;
		animationClip.frameDuration = 0.1f;
		animationClip.frames.reserve(4); 
		switch (m_currentAnimationState) {
		case PlayerAnimationState::IDLE: {
			const auto src = Config::PLAYER_IDLE_SRC;
			animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
			break;	
			}
		case PlayerAnimationState::RUN: {
			const auto src = Config::PLAYER_RUN_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			break;
		}
		case PlayerAnimationState::JUMP: {
			const auto src = Config::PLAYER_JUMP_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			// 跳跃动画不循环
			animationClip.loop = false; 
			break;
		}
		case PlayerAnimationState::FALL: {
			const auto src = Config::PLAYER_FALL_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			// 下降动画不循环
			animationClip.loop = false; 
			break;
		}
		case PlayerAnimationState::ATTACK: {
			const auto src = Config::PLAYER_ATTACK_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			// 根据攻击持续时间和帧数计算每帧持续时间
			animationClip.frameDuration = m_attackDuration / src.size(); 
			// 攻击动画不循环
			animationClip.loop = false; 
			break;
		}
		case PlayerAnimationState::SPRINT: {
			const auto src = Config::PLAYER_SPRINT_SRC; 
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			break;
		}
		case PlayerAnimationState::HIT: {
			const auto src = Config::PLAYER_HITED_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			break;
		}
		case PlayerAnimationState::WIN: {
			const auto src = Config::PLAYER_CELEBRATE_SRC; 
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			animationClip.frameDuration = 0.3f;
			break;
		}
		default:{
			const auto src = Config::PLAYER_IDLE_SRC;
			animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
			break;
		}
		}
		m_animation.play(animationClip); 
	}
	m_animation.update(dt);
}

void Player::postPhysicsUpdate() noexcept{
	// 根据物理系统更新后的状态进行最终调整
	// 土狼时间
	if (m_wasLanded && !m_rigidBody.isLanded && m_rigidBody.velocity.y() >= 0.0f) {
		m_coyoteTimer.start(Config::COYOTE_TIME);
	}
	if(m_rigidBody.isLanded) {
		m_coyoteTimer.stop();
		m_isJumping = false; 
	}
	m_wasLanded = m_rigidBody.isLanded;
	// 跳跃结束
}

void Player::setHit() noexcept{
	if (!m_isHited) {
		m_isHited = true;
		if (--m_hp <= 0) {
			dead();
		}
	}
}

void Player::updatePublicStatus(double dt) noexcept{
	// 根据输入确定移动方向，优先级：右 > 左 > 无
	int dir = (m_command.moveRight ? 1 : m_command.moveLeft ? -1 : 0); 
	// 根据输入更新朝向，优先级：右 > 左 > 保持当前
	m_facingRight = (dir > 0) ? true : (dir < 0) ? false : m_facingRight; 

	// 冲刺方向：输入方向优先，如果没有输入则根据当前朝向决定水平冲刺方向，垂直方向根据up down输入决定，如果没有输入则不进行垂直冲刺
	// 无输入时：水平冲刺方向根据当前朝向决定，垂直冲刺方向不进行冲刺
	// 只有垂直方向时：根据up down输入决定垂直冲刺方向，水平冲刺方向不进行冲刺
	// 水平和垂直方向同时有输入时：水平冲刺方向根据输入决定，垂直冲刺方向根据输入决定
	if (m_command.up || m_command.down) {
		m_sprintDirection.setY(m_command.up ? -1 : m_command.down ? 1 : 0);
		m_sprintDirection.setX(dir);
	} else {
		// 没有垂直输入时，水平冲刺方向根据dir或者当前朝向决定
		m_sprintDirection.setX(dir == 0 ? (m_facingRight ? 1 : -1) : dir); 
		m_sprintDirection.setY(0);
	}

	if (m_command.jump) {
		// 启动下落缓冲计时，允许在平台上短暂时间内仍然可以下落
		m_dropDownTimer.start(Config::DROP_DOWN_BUFFER_TIME); 
	}

	// 根据输入更新希望下落标志，物理系统会根据这个标志处理平台穿透逻辑
	m_rigidBody.wantsDropDown = m_command.down && m_dropDownTimer.isActive(); 

	if (m_rigidBody.isLanded) {
		// 在地面上重置冲刺次数
		m_sprintCount = 0; 
	}

	if (m_isHited && !m_hitTimer.isActive()) {
		m_sfxToPlay.push_back(SfxId::PLAYER_HURT);
		m_hitTimer.start(Config::HIT_TIMER);
		setIsCollidable(false);
	}

	m_jumpTimer.update(dt);
	m_coyoteTimer.update(dt);
	m_attackTimer.update(dt);
	m_sprintTimer.update(dt);
	m_dropDownTimer.update(dt);
	m_hitTimer.update(dt);

	m_isUP = m_command.up;
	m_isDown = m_command.down;

	if(m_command.jump && !m_command.down) {
		// 启动跳跃缓冲计时，允许在离地面短暂时间内仍然可以跳跃
		m_jumpTimer.start(Config::JUMP_BUFFER_TIME); 
	}

	// 判断是否可以攀爬
	if(m_moveMode == MoveMode::CLIMB && m_command.up) {
		m_isClimbing = true;
	} else {
		m_isClimbing = false;
	}

	// 冲刺逻辑，在攀爬后调用
	if (m_command.sprint && !m_isSprinting && m_sprintCount < m_sprintMaxCount) {
		m_sfxToPlay.push_back(SfxId::PLAYER_SPRINT);
		m_moveMode = MoveMode::SPRINT;
		m_isSprinting = true;
		m_sprintTimer.start(Config::SPRINT_DURATION); 
		m_sprintCount++;
	}

	if (m_jumpTimer.isActive() && (m_coyoteTimer.isActive() || m_rigidBody.isLanded)) {
		m_jumpRequested = true;
		m_isJumping = true;
		m_rigidBody.isLanded = false;
		m_jumpTimer.stop(); 
		m_coyoteTimer.stop();
	}

	if (m_command.attack && !m_isAttacking) {
		m_sfxToPlay.push_back(SfxId::PLAYER_ATTACK_SWING);
		m_isAttacking = true;
		m_attackTimer.start(m_attackDuration);
		m_attackHitBox.setY(m_rigidBody.hitBox.y());
		// 根据朝向调整攻击碰撞盒位置
		m_attackHitBox.setX(m_rigidBody.hitBox.x() + (m_facingRight ? 1 : -1) * m_rigidBody.hitBox.w()); 
		m_attackFacingRight = m_facingRight; 
	}
}

void Player::updateMovementAcceleration(double dt) noexcept{
	int dir = (m_command.moveRight ? 1 : m_command.moveLeft ? -1 : 0); // 根据输入确定移动方向，优先级：右 > 左 > 无

	switch (m_moveMode) {
	case MoveMode::SPRINT: {
		if (m_isSprinting) {
			m_rigidBody.acceleration.set(0, 0); 
			m_rigidBody.velocity = m_sprintDirection.normalized() * Config::SPRINT_MAX_SPEED;
			// 冲刺时不受重力影响
			m_rigidBody.gravityScale = 0.0f; 
			m_rigidBody.maxSpeed = Config::SPRINT_MAX_SPEED; 
			if (m_sprintDirection.y() >= -0.1f && m_sprintDirection.y() <= 0.1f) { 
				m_rigidBody.isLanded = false;
			}
		}
		break;
	}
	case MoveMode::CLIMB: {
		// 暂时不实现
		break;
	}
	case MoveMode::NORMAL :{
		m_rigidBody.gravityScale = 1.0f; 
		m_rigidBody.maxSpeed = Config::MAX_SPEED; 
		m_rigidBody.maxFallSpeed = Config::MAX_SPEED; 

		// 摩擦在此处结算（为调整手感使用，而不是通用摩擦力）
		if (dir != 0) { 
			m_rigidBody.acceleration.setX(dir * (m_rigidBody.isLanded ? Config::ACCELERATION : Config::AIR_ACCEL)); 
		} else if (m_rigidBody.isLanded) {
			// 无输入 + 在地面：摩擦减速
			float vx = m_rigidBody.velocity.x();
			if (std::abs(vx) > Config::GROUND_FRICTION * dt) {
				m_rigidBody.acceleration.setX(-std::copysign(Config::GROUND_FRICTION, vx));
			} else {
				// 积分前归零，防止抖动
				m_rigidBody.velocity.setX(0.0f);   
				m_rigidBody.acceleration.setX(0.0f); 
			}
		} else {
			if (m_isDown) {
				m_rigidBody.acceleration.setY(0.5 * Config::GRAVITY);
				// 按下下键时增加最大下落速度，允许更快地落地
				m_rigidBody.maxFallSpeed = Config::MAX_FALL_SPEED_WHILE_DOWN; 
			}
			// 无输入 + 在空中：不施加水平加速度，保持当前水平速度（不受空气摩擦影响）
			m_rigidBody.acceleration.setX(0.0f);
		}
		break;
	}

	default:
		break;
	}
	if(m_jumpRequested) {
		m_sfxToPlay.push_back(SfxId::PLAYER_JUMP);
		m_rigidBody.velocity.setY(Config::JUMP_VELOCITY);
		m_jumpRequested = false; 
	}
}

void Player::finalizeState() noexcept{
	// 冲刺结束
	if (m_isSprinting && !m_sprintTimer.isActive()) {
		m_isSprinting = false;
		m_sprintDirection = Vec2(0, 0);
		m_moveMode = MoveMode::NORMAL; 
	}
	// 攻击结束
	if (m_isAttacking && !m_attackTimer.isActive()) {
		m_isAttacking = false;
	}

	// 着陆时重置跳跃状态，允许再次跳跃
	if (m_rigidBody.isLanded) {
		m_coyoteTimer.stop();
		m_isJumping = false;
	}
	// 受击无敌结束
	if (m_isHited && !m_hitTimer.isActive()) {
		m_isHited = false;
		setIsCollidable(true);
	}
	// 后续根据实际情况调整状态转换的优先级和条件，例如攻击状态可能需要覆盖其他状态，或者增加更多的状态细分等
}

void Player::judgeClimb(bool canClimb) noexcept{
	if (m_moveMode == MoveMode::SPRINT) {
		return;
	}
	m_moveMode = canClimb ? MoveMode::CLIMB : MoveMode::NORMAL;
}