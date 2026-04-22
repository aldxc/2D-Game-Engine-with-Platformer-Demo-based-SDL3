#include <algorithm>
#include <cmath>
#include <array>
#include "Player.h"
#include "render/Renderer.h"
#include "render/Camera.h"
#include "resource/Resource.h"
#include "physics/Physics.h"
#include "core/Rect.h"

Player::Player(Renderer& renderer, Resource& rM) noexcept : renderer_(renderer){
	playerTexture_ = rM.loadTexture(Config::PLAYER_TEXTURE_PATH, renderer_.getSDLRenderer());
	attackTexture_ = rM.loadTexture(Config::ATTACK_TEXTURE_PATH, renderer_.getSDLRenderer());
	command_ = {};

	currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation::AnimationClip animationClip;
	const auto src = Config::PLAYER_IDLE_SRC;
	animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
	animation_.play(animationClip);

	attackDuration_ = Config::ATTACK_DURATION;
	attackHitBox_ = { 0,0,Config::ATTACK_WIDTH, Config::ATTACK_HEIGHT };
	sprintDirection_ = Vec2(0, 0);
	sprintMaxCount_ = Config::SPRINT_MAX_COUNT;
	sprintCount_ = 0;
	birthPoint_ = Vec2(0, 0);
	rigidBody_ = RigidBody(Vec2(0, 0), Rect(birthPoint_.x(), birthPoint_.y(), Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT), Config::MAX_SPEED);
}

void Player::update(double dt) noexcept{
	updatePublicStatus(dt);

	updateMovementAcceleration(dt);

	finalizeState();
}

void Player::render(const Camera& camera) const noexcept{
	Rect scaleTextureRect = rigidBody_.hitBox;
	// 为了让玩家在屏幕上看起来更大一些，这里对渲染的矩形进行缩放
	scaleTextureRect.setW(scaleTextureRect.w() * 2);
	scaleTextureRect.setH(scaleTextureRect.h() * 2);
	scaleTextureRect.setX(scaleTextureRect.x() - 16);
	scaleTextureRect.setY(scaleTextureRect.y() - 32);

	std::array<float, 3> offsets = { Config::PLAYER_1_Y_OFFSET, Config::PLAYER_2_Y_OFFSET, Config::PLAYER_3_Y_OFFSET };

	Rect currentFrameRect = animation_.getCurrentFrameRect();
	currentFrameRect.setY(currentFrameRect.y() + offsets[playerId_]); 

	scaleTextureRect = camera.worldToScreen(scaleTextureRect);

	// 显示血条
	Rect currentHpRect = Rect{ rigidBody_.hitBox.x(), rigidBody_.hitBox.y() - 20, rigidBody_.hitBox.w() * (hp_ / static_cast<float>(maxHp_)), Config::BLOOD_RECT_HEIGHT };
	Rect healthBarRect = Rect{ rigidBody_.hitBox.x(), rigidBody_.hitBox.y() - 20, rigidBody_.hitBox.w(), Config::BLOOD_RECT_HEIGHT };

	renderer_.renderFillRect(camera.worldToScreen(currentHpRect), SDL_Color({ 255, 0, 0, 255 })); 
	renderer_.renderRect(camera.worldToScreen(healthBarRect), SDL_Color({ 255, 255, 255, 255 }));

	// 受击时闪烁效果，后续可以根据需要调整闪烁频率、持续时间等参数
	// 这里使用正弦函数来实现闪烁效果，alpha值在0.3到1.0之间变化，频率为10Hz
	if (isHited_ && hitTimer_.isActive()) {
		float progress = hitTimer_.getElapsedTime();
		float alpha = 0.3f + 0.7f * (0.5f + 0.5f * std::sin(progress * 10.0f));
		SDL_SetTextureAlphaModFloat(playerTexture_.get(), alpha);
	}

	if (facingRight_) {
		renderer_.renderTexture(playerTexture_.get(), currentFrameRect, scaleTextureRect);
	}
	else {
		renderer_.reversePlayerFaceTexture(playerTexture_.get(), currentFrameRect, scaleTextureRect);
	}

	// 恢复alpha，防止影响其他使用同一纹理的对象
	if (isHited_ && hitTimer_.isActive()) {
		SDL_SetTextureAlphaModFloat(playerTexture_.get(), 1.0f);
	}

	if (isAttacking_) {
		const auto& src = Config::ATTACK_SRC[animation_.getCurrentFrameIndex()];
		Rect attackSrcRect = Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) };
		Rect attackDstRect = camera.worldToScreen(attackHitBox_);

		if (attackFacingRight_) {
			renderer_.renderTexture(attackTexture_.get(), attackSrcRect, attackDstRect);
		}
		else {
			renderer_.reversePlayerFaceTexture(attackTexture_.get(), attackSrcRect, attackDstRect);
		}
	}
}

void Player::renderDebug(const Camera& camera) const noexcept{
	SDL_Color debugTextColor = SDL_Color({ 255, 255, 255, 255 }); // 白色文本
	SDL_FRect debugInfoRect = { Config::LOGIC_WIDTH - 200, 0, 200, 200 };
	std::string str = (rigidBody_.isLanded) ? "Landed" : "Air";
	renderer_.renderText("IsLand:" + str, debugInfoRect, debugTextColor, 20);
	std::string velocityXStr = "VelX: " + std::to_string(static_cast<int>(rigidBody_.velocity.x()));
	std::string velocityYStr = "VelY: " + std::to_string(static_cast<int>(rigidBody_.velocity.y()));
	renderer_.renderText(velocityXStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 30, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	renderer_.renderText(velocityYStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 60, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string currState = "State: ";
	switch (currentAnimationState_) {
	case PlayerAnimationState::IDLE: currState += "Idle"; break;
	case PlayerAnimationState::RUN: currState += "Run"; break;
	case PlayerAnimationState::JUMP: currState += "Jump"; break; 
	case PlayerAnimationState::FALL: currState += "Fall"; break;
	case PlayerAnimationState::ATTACK: currState += "Attack"; break;
	case PlayerAnimationState::CLIMB: currState += "Climb"; break;
	case PlayerAnimationState::SPRINT: currState += "Sprint"; break;
	case PlayerAnimationState::HIT: currState += "Hited"; break;
	}
	renderer_.renderText(currState, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 90, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string playerPosStr = "Pos: (" + std::to_string(static_cast<int>(rigidBody_.hitBox.x())) + ", " + std::to_string(static_cast<int>(rigidBody_.hitBox.y())) + ")";
	renderer_.renderText(playerPosStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 120, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string cameraMoveStr = "CameraMove: (" + std::to_string(static_cast<int>(camera.getViewport().x())) + ", " + std::to_string(static_cast<int>(camera.getViewport().y())) + ")";
	renderer_.renderText(cameraMoveStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 150, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string moveModeStr = "MoveMode: " + std::string((moveMode_ == MoveMode::NORMAL) ? "Normal" : (moveMode_ == MoveMode::CLIMB) ? "Climb" : "Sprint");
	renderer_.renderText(moveModeStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 180, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string hit = "is hited: " + std::string((isHited_) ? "true" : "false");
	renderer_.renderText(hit, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 210, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	renderer_.renderRect(camera.worldToScreen(rigidBody_.hitBox), SDL_Color({255, 0, 0, 255})); // 用红色矩形表示玩家碰撞盒
	renderer_.renderRect(camera.worldToScreen(attackHitBox_), (isAttacking_ ? renderer_.setColorAlpha(255, 0, 0, 255) : renderer_.setColorAlpha(200, 200, 200, 255)));
}

void Player::reset() noexcept {
	rigidBody_.velocity = Vec2(0, 0);
	rigidBody_.hitBox = Rect(birthPoint_.x(), birthPoint_.y(), Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT);
	rigidBody_.isLanded = false;
	rigidBody_.acceleration = Vec2(0, 0);
	rigidBody_.gravityScale = 1.0f;
	rigidBody_.maxSpeed = Config::MAX_SPEED;

	facingRight_ = true;
	isJumping_ = false;
	jumpRequested_ = false;
	isAttacking_ = false;
	isSprinting_ = false;
	wasLanded_ = false;
	isHited_ = false;
	isDead_ = false;
	sprintDirection_ = Vec2(0, 0);
	sprintCount_ = 0;
	hp_ = Config::PLAYER_HP;
	isWinner_ = false;
}

bool Player::isStateChanged() noexcept{
	PlayerAnimationState nextState = PlayerAnimationState::IDLE;
	if (rigidBody_.isLanded) {
		if (rigidBody_.velocity.x() <= 0.1f && rigidBody_.velocity.x() >= -0.1f) {
			nextState = PlayerAnimationState::IDLE;
		}
		else 
			nextState = PlayerAnimationState::RUN;
	}
	else {
		if (rigidBody_.velocity.y() < -0.1f) {
			nextState = PlayerAnimationState::JUMP;
		}
		else if (rigidBody_.velocity.y() > 0.1f) {
			nextState = PlayerAnimationState::FALL;
		}
		else {
			nextState = PlayerAnimationState::FALL;
		}
	}
	if (isSprinting_) {
		nextState = PlayerAnimationState::SPRINT;
	}
	if (isAttacking_) {
		nextState = PlayerAnimationState::ATTACK;
	}
	if(isHited_ && !hitTimer_.isActive()) {
		nextState = PlayerAnimationState::HIT;
	}
	if(isWinner_) {
		nextState = PlayerAnimationState::WIN;
	}
	bool f = (nextState != currentAnimationState_);
	currentAnimationState_ = nextState;
	return f;
}

void Player::updateAnimationState(double dt) noexcept{
	if (isStateChanged()) {
		Animation::AnimationClip animationClip;
		animationClip.frameDuration = 0.1f;
		animationClip.frames.reserve(4); 
		switch (currentAnimationState_) {
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
			animationClip.frameDuration = attackDuration_ / src.size(); 
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
		animation_.play(animationClip); 
	}
	animation_.update(dt);
}

void Player::postPhysicsUpdate() noexcept{
	// 根据物理系统更新后的状态进行最终调整
	// 土狼时间
	if (wasLanded_ && !rigidBody_.isLanded && rigidBody_.velocity.y() >= 0.0f) {
		coyoteTimer_.start(Config::COYOTE_TIME);
	}
	if(rigidBody_.isLanded) {
		coyoteTimer_.stop();
		isJumping_ = false; 
	}
	wasLanded_ = rigidBody_.isLanded;
	// 跳跃结束
}

void Player::setHit() noexcept{
	if (!isHited_) {
		isHited_ = true;
		if (--hp_ <= 0) {
			dead();
		}
	}
}

void Player::updatePublicStatus(double dt) noexcept{
	// 根据输入确定移动方向，优先级：右 > 左 > 无
	int dir = (command_.moveRight ? 1 : command_.moveLeft ? -1 : 0); 
	// 根据输入更新朝向，优先级：右 > 左 > 保持当前
	facingRight_ = (dir > 0) ? true : (dir < 0) ? false : facingRight_; 

	// 冲刺方向：输入方向优先，如果没有输入则根据当前朝向决定水平冲刺方向，垂直方向根据up down输入决定，如果没有输入则不进行垂直冲刺
	// 无输入时：水平冲刺方向根据当前朝向决定，垂直冲刺方向不进行冲刺
	// 只有垂直方向时：根据up down输入决定垂直冲刺方向，水平冲刺方向不进行冲刺
	// 水平和垂直方向同时有输入时：水平冲刺方向根据输入决定，垂直冲刺方向根据输入决定
	if (command_.up || command_.down) {
		sprintDirection_.setY(command_.up ? -1 : command_.down ? 1 : 0);
		sprintDirection_.setX(dir);
	} else {
		// 没有垂直输入时，水平冲刺方向根据dir或者当前朝向决定
		sprintDirection_.setX(dir == 0 ? (facingRight_ ? 1 : -1) : dir); 
		sprintDirection_.setY(0);
	}

	if (command_.jump) {
		// 启动下落缓冲计时，允许在平台上短暂时间内仍然可以下落
		dropDownTimer_.start(Config::DROP_DOWN_BUFFER_TIME); 
	}

	// 根据输入更新希望下落标志，物理系统会根据这个标志处理平台穿透逻辑
	rigidBody_.wantsDropDown = command_.down && dropDownTimer_.isActive(); 

	if (rigidBody_.isLanded) {
		// 在地面上重置冲刺次数
		sprintCount_ = 0; 
	}

	if (isHited_ && !hitTimer_.isActive()) {
		sfxToPlay_.push_back(SfxId::PLAYER_HURT);
		hitTimer_.start(Config::HIT_TIMER);
		setIsCollidable(false);
	}

	jumpTimer_.update(dt);
	coyoteTimer_.update(dt);
	attackTimer_.update(dt);
	sprintTimer_.update(dt);
	dropDownTimer_.update(dt);
	hitTimer_.update(dt);

	isUP_ = command_.up;
	isDown_ = command_.down;

	if(command_.jump && !command_.down) {
		// 启动跳跃缓冲计时，允许在离地面短暂时间内仍然可以跳跃
		jumpTimer_.start(Config::JUMP_BUFFER_TIME); 
	}

	// 判断是否可以攀爬
	if(moveMode_ == MoveMode::CLIMB && command_.up) {
		isClimbing_ = true;
	} else {
		isClimbing_ = false;
	}

	// 冲刺逻辑，在攀爬后调用
	if (command_.sprint && !isSprinting_ && sprintCount_ < sprintMaxCount_) {
		sfxToPlay_.push_back(SfxId::PLAYER_SPRINT);
		moveMode_ = MoveMode::SPRINT;
		isSprinting_ = true;
		sprintTimer_.start(Config::SPRINT_DURATION); 
		sprintCount_++;
	}

	if (jumpTimer_.isActive() && (coyoteTimer_.isActive() || rigidBody_.isLanded)) {
		jumpRequested_ = true;
		isJumping_ = true;
		rigidBody_.isLanded = false;
		jumpTimer_.stop(); 
		coyoteTimer_.stop();
	}

	if (command_.attack && !isAttacking_) {
		sfxToPlay_.push_back(SfxId::PLAYER_ATTACK_SWING);
		isAttacking_ = true;
		attackTimer_.start(attackDuration_);
		attackHitBox_.setY(rigidBody_.hitBox.y());
		// 根据朝向调整攻击碰撞盒位置
		attackHitBox_.setX(rigidBody_.hitBox.x() + (facingRight_ ? 1 : -1) * rigidBody_.hitBox.w()); 
		attackFacingRight_ = facingRight_; 
	}
}

void Player::updateMovementAcceleration(double dt) noexcept{
	int dir = (command_.moveRight ? 1 : command_.moveLeft ? -1 : 0); // 根据输入确定移动方向，优先级：右 > 左 > 无

	switch (moveMode_) {
	case MoveMode::SPRINT: {
		if (isSprinting_) {
			rigidBody_.acceleration.set(0, 0); 
			rigidBody_.velocity = sprintDirection_.normalized() * Config::SPRINT_MAX_SPEED;
			// 冲刺时不受重力影响
			rigidBody_.gravityScale = 0.0f; 
			rigidBody_.maxSpeed = Config::SPRINT_MAX_SPEED; 
			if (sprintDirection_.y() >= -0.1f && sprintDirection_.y() <= 0.1f) { 
				rigidBody_.isLanded = false;
			}
		}
		break;
	}
	case MoveMode::CLIMB: {
		// 暂时不实现
		break;
	}
	case MoveMode::NORMAL :{
		rigidBody_.gravityScale = 1.0f; 
		rigidBody_.maxSpeed = Config::MAX_SPEED; 
		rigidBody_.maxFallSpeed = Config::MAX_SPEED; 

		// 摩擦在此处结算（为调整手感使用，而不是通用摩擦力）
		if (dir != 0) { 
			rigidBody_.acceleration.setX(dir * (rigidBody_.isLanded ? Config::ACCELERATION : Config::AIR_ACCEL)); 
		} else if (rigidBody_.isLanded) {
			// 无输入 + 在地面：摩擦减速
			float vx = rigidBody_.velocity.x();
			if (std::abs(vx) > Config::GROUND_FRICTION * dt) {
				rigidBody_.acceleration.setX(-std::copysign(Config::GROUND_FRICTION, vx));
			} else {
				// 积分前归零，防止抖动
				rigidBody_.velocity.setX(0.0f);   
				rigidBody_.acceleration.setX(0.0f); 
			}
		} else {
			if (isDown_) {
				rigidBody_.acceleration.setY(0.5 * Config::GRAVITY);
				// 按下下键时增加最大下落速度，允许更快地落地
				rigidBody_.maxFallSpeed = Config::MAX_FALL_SPEED_WHILE_DOWN; 
			}
			// 无输入 + 在空中：不施加水平加速度，保持当前水平速度（不受空气摩擦影响）
			rigidBody_.acceleration.setX(0.0f);
		}
		break;
	}

	default:
		break;
	}
	if(jumpRequested_) {
		sfxToPlay_.push_back(SfxId::PLAYER_JUMP);
		rigidBody_.velocity.setY(Config::JUMP_VELOCITY);
		jumpRequested_ = false; 
	}
}

void Player::finalizeState() noexcept{
	// 冲刺结束
	if (isSprinting_ && !sprintTimer_.isActive()) {
		isSprinting_ = false;
		sprintDirection_ = Vec2(0, 0);
		moveMode_ = MoveMode::NORMAL; 
	}
	// 攻击结束
	if (isAttacking_ && !attackTimer_.isActive()) {
		isAttacking_ = false;
	}

	// 着陆时重置跳跃状态，允许再次跳跃
	if (rigidBody_.isLanded) {
		coyoteTimer_.stop();
		isJumping_ = false;
	}
	// 受击无敌结束
	if (isHited_ && !hitTimer_.isActive()) {
		isHited_ = false;
		setIsCollidable(true);
	}
	// 后续根据实际情况调整状态转换的优先级和条件，例如攻击状态可能需要覆盖其他状态，或者增加更多的状态细分等
}

void Player::judgeClimb(bool canClimb) noexcept{
	if (moveMode_ == MoveMode::SPRINT) {
		return;
	}
	moveMode_ = canClimb ? MoveMode::CLIMB : MoveMode::NORMAL;
}