#include <algorithm>
#include <cmath>
#include "Player.h"
#include "render/Renderer.h"
#include "resource/Resource.h"
#include "physics/Physics.h"
#include "core/Rect.h"

Player::Player(Renderer& renderer, Resource& rM) noexcept : renderer_(renderer){
	playerTexture_ = rM.loadTexture("resource/characters.png", renderer_.getSDLRenderer());
	attackTexture_ = rM.loadTexture("resource/swoosh.png", renderer_.getSDLRenderer());
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
	rigidBody_ = RigidBody(Vec2(0, 0), Rect(0, 0, Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT), Config::MAX_SPEED);
}

void Player::update(float dt) noexcept{
	updatePublicStatus(dt);

	updateMovementAcceleration(dt);

	finalizeState();
}

void Player::render(const Camera& camera) const noexcept{
	Rect scaleTextureRect = rigidBody_.hitBox;
	scaleTextureRect.setW(scaleTextureRect.w() * 2);
	scaleTextureRect.setH(scaleTextureRect.h() * 2);
	scaleTextureRect.setX(scaleTextureRect.x() - 16);
	scaleTextureRect.setY(scaleTextureRect.y() - 32);

	Rect currentFrameRect = animation_.getCurrentFrameRect();
	currentFrameRect.setY(currentFrameRect.getY() + Config::PLAYER_2_Y_OFFSET); // 根据玩家编号调整动画状态的y坐标，支持多个玩家使用同一纹理图

	scaleTextureRect = camera.worldToScreen(scaleTextureRect); // 将玩家的世界坐标转换为屏幕坐标
	if (facingRight_) {
		renderer_.renderTexture(playerTexture_.get(), currentFrameRect, scaleTextureRect);
	}
	else {
		renderer_.reversePlayerFaceTexture(playerTexture_.get(), currentFrameRect, scaleTextureRect);
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
	std::string velocityXStr = "VelX: " + std::to_string(static_cast<int>(rigidBody_.velocity.getX()));
	std::string velocityYStr = "VelY: " + std::to_string(static_cast<int>(rigidBody_.velocity.getY()));
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
	}
	renderer_.renderText(currState, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 90, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string playerPosStr = "Pos: (" + std::to_string(static_cast<int>(rigidBody_.hitBox.getX())) + ", " + std::to_string(static_cast<int>(rigidBody_.hitBox.getY())) + ")";
	renderer_.renderText(playerPosStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 120, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string cameraMoveStr = "CameraMove: (" + std::to_string(static_cast<int>(camera.getViewport().getX())) + ", " + std::to_string(static_cast<int>(camera.getViewport().getY())) + ")";
	renderer_.renderText(cameraMoveStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 150, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string moveModeStr = "MoveMode: " + std::string((moveMode_ == MoveMode::Normal) ? "Normal" : (moveMode_ == MoveMode::Climb) ? "Climb" : "Sprint");
	renderer_.renderText(moveModeStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 180, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);

	renderer_.renderRect(camera.worldToScreen(rigidBody_.hitBox), SDL_Color({255, 0, 0, 255})); // 用红色矩形表示玩家碰撞盒
}

void Player::reset() noexcept {
	rigidBody_.velocity = Vec2(0, 0);
	rigidBody_.hitBox = Rect(0, 0, Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT);
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
}

bool Player::isStateChanged() noexcept{
	PlayerAnimationState nextState = PlayerAnimationState::IDLE;
	if (rigidBody_.isLanded) {
		if (rigidBody_.velocity.getX() <= 0.1f && rigidBody_.velocity.getX() >= -0.1f) {
			nextState = PlayerAnimationState::IDLE;
		}
		else 
			nextState = PlayerAnimationState::RUN;
	}
	else {
		if (rigidBody_.velocity.getY() < -0.1f) {
			nextState = PlayerAnimationState::JUMP;
		}
		else if (rigidBody_.velocity.getY() > 0.1f) {
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
	bool f = (nextState != currentAnimationState_);
	currentAnimationState_ = nextState;
	return f;
}

void Player::updateAnimationState(float dt) noexcept{
	if (isStateChanged()) {
		Animation::AnimationClip animationClip;
		animationClip.frameDuration = 0.1f;
		animationClip.frames.reserve(4); // 预分配空间，避免多次动态扩容，后续可以根据实际帧数调整
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
			animationClip.loop = false; // 跳跃动画不循环，播放完后停在最后一帧，直到状态改变
			break;
		}
		case PlayerAnimationState::FALL: {
			const auto src = Config::PLAYER_FALL_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			animationClip.loop = false; // 下降动画不循环，播放完后停在最后一帧，直到状态改变
			break;
		}
		case PlayerAnimationState::ATTACK: {
			const auto src = Config::PLAYER_ATTACK_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			animationClip.frameDuration = attackDuration_ / src.size(); // 根据攻击持续时间和帧数计算每帧持续时间
			animationClip.loop = false; // 攻击动画不循环，播放完后停在最后一帧，直到状态改变
			break;
		}
		case PlayerAnimationState::SPRINT: {
			const auto src = Config::PLAYER_SPRINT_SRC; // 冲刺动画暂时使用跑动动画帧，后续可以添加专门的冲刺动画帧
			for (const auto& rect : src) {
				animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			break;
		}
		default:{
			const auto src = Config::PLAYER_IDLE_SRC;
			animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
			break;
		}
		}
		animation_.play(animationClip); // 根据当前状态获取对应的动画剪辑并播放
	}
	animation_.update(dt);
}

void Player::postPhysicsUpdate() noexcept{
	// 根据物理系统更新后的状态进行最终调整
	// 土狼时间
	if (wasLanded_ && !rigidBody_.isLanded && rigidBody_.velocity.getY() >= 0.0f) {
		coyoteTimer_.start(Config::COYOTE_TIME);
	}
	if(rigidBody_.isLanded) {
		coyoteTimer_.stop();
		isJumping_ = false; // 着陆后重置跳跃状态
	}
	wasLanded_ = rigidBody_.isLanded;
	// 跳跃结束
}

void Player::updatePublicStatus(float dt) noexcept{
	int dir = (command_.moveRight ? 1 : command_.moveLeft ? -1 : 0); // 根据输入确定移动方向，优先级：右 > 左 > 无
	facingRight_ = (dir > 0) ? true : (dir < 0) ? false : facingRight_; // 根据输入更新朝向，优先级：右 > 左 > 保持当前

	// 冲刺方向：输入方向优先，如果没有输入则根据当前朝向决定水平冲刺方向，垂直方向根据up down输入决定，如果没有输入则不进行垂直冲刺
	// 无输入时：水平冲刺方向根据当前朝向决定，垂直冲刺方向不进行冲刺
	// 只有垂直方向时：根据up down输入决定垂直冲刺方向，水平冲刺方向不进行冲刺
	// 水平和垂直方向同时有输入时：水平冲刺方向根据输入决定，垂直冲刺方向根据输入决定
	if (command_.up || command_.down) {
		sprintDirection_.setY(command_.up ? -1 : command_.down ? 1 : 0);
		sprintDirection_.setX(dir);
	} else {
		sprintDirection_.setX(dir == 0 ? (facingRight_ ? 1 : -1) : dir); // 没有垂直输入时，水平冲刺方向根据dir或者当前朝向决定
		sprintDirection_.setY(0);
	}

	if (command_.jump) {
		dropDownTimer_.start(Config::DROP_DOWN_BUFFER_TIME); // 启动下落缓冲计时，允许在平台上短暂时间内仍然可以下落
	}

	rigidBody_.wantsDropDown = command_.down && dropDownTimer_.isActive(); // 根据输入更新希望下落标志，物理系统会根据这个标志处理平台穿透逻辑

	if (rigidBody_.isLanded) {
		sprintCount_ = 0; // 在地面上重置冲刺次数
	}

	jumpTimer_.update(dt);
	coyoteTimer_.update(dt);
	attackTimer_.update(dt);
	sprintTimer_.update(dt);
	dropDownTimer_.update(dt);

	isUP_ = command_.up;

	if(command_.jump && !command_.down) {
		jumpTimer_.start(Config::JUMP_BUFFER_TIME); // 启动跳跃缓冲计时，允许在离地面短暂时间内仍然可以跳跃
	}

	// 判断是否可以攀爬
	if(moveMode_ == MoveMode::Climb && command_.up) {
		isClimbing_ = true;
	} else {
		isClimbing_ = false;
	}

	// 冲刺逻辑，再攀爬后调用
	if (command_.sprint && !isSprinting_ && sprintCount_ < sprintMaxCount_) {
		moveMode_ = MoveMode::Sprint;
		isSprinting_ = true;
		sprintTimer_.start(Config::SPRINT_DURATION); // 冲刺持续时间，单位秒
		sprintCount_++;
	}

	if (jumpTimer_.isActive() && (coyoteTimer_.isActive() || rigidBody_.isLanded)) {
		jumpRequested_ = true;
		isJumping_ = true;
		rigidBody_.isLanded = false;
		jumpTimer_.stop(); // 跳跃后停止跳跃缓冲计时
		coyoteTimer_.stop(); //
	}

	if (command_.attack && !isAttacking_) {
		isAttacking_ = true;
		attackTimer_.start(attackDuration_);
		attackHitBox_.setY(rigidBody_.hitBox.getY());
		attackHitBox_.setX(rigidBody_.hitBox.getX() + (facingRight_ ? 1 : -1) * rigidBody_.hitBox.getW()); // 根据朝向调整攻击碰撞盒位置
		attackFacingRight_ = facingRight_; // 攻击时的朝向快照
	}
}

void Player::updateMovementAcceleration(float dt) noexcept{
	int dir = (command_.moveRight ? 1 : command_.moveLeft ? -1 : 0); // 根据输入确定移动方向，优先级：右 > 左 > 无

	switch (moveMode_) {
	case MoveMode::Sprint: {
		if (isSprinting_) {
			rigidBody_.acceleration.set(0, 0); // 冲刺时不受输入加速度影响，保持当前速度
			rigidBody_.velocity = sprintDirection_.normalized() * Config::SPRINT_MAX_SPEED;
			rigidBody_.gravityScale = 0.0f; // 冲刺时不受重力影响
			rigidBody_.maxSpeed = Config::SPRINT_MAX_SPEED; // 冲刺时的最大速度
			if (sprintDirection_.getY() >= -0.1f && sprintDirection_.getY() <= 0.1f) { // 水平冲刺时保持着陆状态，允许在地面上进行水平冲刺
				rigidBody_.isLanded = false;
			}
		}
		break;
	}
	case MoveMode::Climb: {
		// 暂时不实现
		break;
	}
	case MoveMode::Normal :{
		rigidBody_.gravityScale = 1.0f; // 恢复正常重力
		rigidBody_.maxSpeed = Config::MAX_SPEED; // 恢复正常最大速度

		// 摩擦在此处结算（为调整手感使用，而不是通用摩擦力）
		if (dir != 0) {
			rigidBody_.acceleration.setX(dir * (rigidBody_.isLanded ? Config::ACCELERATION : Config::AIR_ACCEL)); // 有输入 根据是否着陆选择不同的加速度
		} else if (rigidBody_.isLanded) {
			// 无输入 + 在地面：摩擦减速
			float vx = rigidBody_.velocity.getX();
			if (std::abs(vx) > Config::GROUND_FRICTION * dt) {
				rigidBody_.acceleration.setX(-std::copysign(Config::GROUND_FRICTION, vx));
			}
			else {
				rigidBody_.velocity.setX(0.0f);   // 积分前归零，防止抖动
				rigidBody_.acceleration.setX(0.0f); // 已归零，不再施加加速度
			}
		} else {
			// 无输入 + 在空中：不施加水平加速度，保持当前水平速度（不受空气摩擦影响）
			rigidBody_.acceleration.setX(0.0f);
		}
		break;
	}

	default:
		break;
	}
	if(jumpRequested_) {
		rigidBody_.velocity.setY(Config::JUMP_VELOCITY); // 跳跃初始速度，单位像素/秒
		jumpRequested_ = false; // 重置跳跃请求
	}
}

void Player::finalizeState() noexcept{
	// 冲刺结束
	if (isSprinting_ && !sprintTimer_.isActive()) {
		isSprinting_ = false;
		sprintDirection_ = Vec2(0, 0);
		moveMode_ = MoveMode::Normal; // 冲刺结束后重置移动模式
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
	// 后续根据实际情况调整状态转换的优先级和条件，例如攻击状态可能需要覆盖其他状态，或者增加更多的状态细分等
}
