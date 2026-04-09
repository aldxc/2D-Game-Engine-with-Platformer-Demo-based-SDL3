#include <algorithm>
#include "Player.h"
#include "render/Renderer.h"
#include "resource/Resource.h"
#include "physics/Physics.h"

Player::Player() {
	playerTexture_ = Resource::getInstance().loadTexture("resource/characters.png", Renderer::getInstance().getSDLRenderer());
	command_ = {};
	currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation::AnimationClip animationClip;
	const auto src = Config::PLAYER_IDLE_SRC;
	animationClip.frames.push_back(SDL_FRect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
	animation_.play(animationClip);
	
}

void Player::update(float dt) noexcept{
	//后续整合至物理系统中
	//只负责更新速度，位置的更新由外部根据碰撞结果调用applyResolvedMovement来处理，保持Player类的单一职责
	int dir = (command_.moveRight ? 1 : command_.moveLeft ? -1 : 0); // 根据输入确定移动方向，优先级：右 > 左 > 无
	facingRight_ = (dir > 0) ? true : (dir < 0) ? false : facingRight_; // 根据输入更新朝向，优先级：右 > 左 > 保持当前

	if(dir != 0) {
		if (isLanded_) {
			if(dir * velocityX_ >= 0)
				velocityX_ += dir * Config::ACCELERATION * dt; // 地面加速
			else 
				velocityX_ += dir * Config::DECELERATION * dt; // 反向输入时先减速到0再加速
		}
		else velocityX_ += dir * Config::AIR_ACCEL * dt; // 空中加速
		velocityX_ = std::clamp(velocityX_, -Config::MAX_SPEED, Config::MAX_SPEED); // 限制最大速度
	} else {
		if (isLanded_) {
			if (velocityX_ > 0)
				velocityX_ = std::max(velocityX_ - Config::GROUND_FRICTION * dt, 0.0f); // 没有输入时减速
			else if (velocityX_ < 0)
				velocityX_ = std::min(velocityX_ + Config::GROUND_FRICTION * dt, 0.0f);
		}
	}

	if(command_.jump) {
		jumpTimer_.start(Config::JUMP_BUFFER_TIME); // 开始跳跃缓冲计时
	}

	jumpTimer_.update(dt); // 更新跳跃缓冲计时
	coyoteTimer_.update(dt); // 更新土狼时间计时

	if (jumpTimer_.isActive() && (coyoteTimer_.isActive() || isLanded_)) {
		velocityY_ = Config::JUMP_VELOCITY; // 跳跃初始速度
		isJumping_ = true;
		isLanded_ = false;
		jumpTimer_.stop(); // 跳跃后停止跳跃缓冲计时
		coyoteTimer_.stop(); //
	}

}

void Player::applyResolvedMovement(const SDL_FRect& hitBox, float velocityX, float velocityY, bool landed) noexcept {
	setHitBox(hitBox);
	velocityX_ = velocityX;
	velocityY_ = velocityY;

	if (isLanded_ && !landed) {
		coyoteTimer_.start(Config::COYOTE_TIME);
	}

	isLanded_ = landed;
	if (landed) {
		coyoteTimer_.stop();
		isJumping_ = false;
	}
}


void Player::render() const noexcept{
	SDL_FRect scaleTextureRect = getHitBox();
	scaleTextureRect.w *= 2;
	scaleTextureRect.h *= 2;
	scaleTextureRect.x -= 16;
	scaleTextureRect.y -= 32;

	SDL_FRect currentFrameRect = animation_.getCurrentFrameRect();
	currentFrameRect.y += Config::PLAYER_3_Y_OFFSET; // 根据玩家编号调整帧的y坐标，支持多个玩家使用同一纹理图

	if (facingRight_)
		Renderer::getInstance().renderTexture(playerTexture_.get(), currentFrameRect, scaleTextureRect);
	else
		Renderer::getInstance().reversePlayerFaceTexture(playerTexture_.get(), currentFrameRect, scaleTextureRect);
}

void Player::renderDebug() const noexcept{
	SDL_Color debugTextColor = SDL_Color({ 255, 255, 255, 255 }); // 白色文本
	SDL_FRect debugInfoRect = { Config::LOGIC_WIDTH - 200, 0, 200, 200 };
	std::string str = (isLanded_) ? "Landed" : "Air";
	Renderer::getInstance().renderText("IsLand:" + str, debugInfoRect, debugTextColor, 20);
	std::string velocityXStr = "VelX: " + std::to_string(static_cast<int>(velocityX_));
	std::string velocityYStr = "VelY: " + std::to_string(static_cast<int>(velocityY_));
	Renderer::getInstance().renderText(velocityXStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 30, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	Renderer::getInstance().renderText(velocityYStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 60, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string currState = "State: ";
	switch (currentAnimationState_) {
	case PlayerAnimationState::IDLE: currState += "Idle"; break;
	case PlayerAnimationState::RUN: currState += "Run"; break;
	case PlayerAnimationState::JUMP: currState += "Jump"; break;
	case PlayerAnimationState::FALL: currState += "Fall"; break;
	}
	Renderer::getInstance().renderText(currState, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 90, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string JumpBufferStr = "JumpBuffer: ";
	JumpBufferStr += (jumpTimer_.isActive() ? "Active" : "Inactive");
	Renderer::getInstance().renderText(JumpBufferStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 120, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);
	std::string CoyoteBufferStr = "CoyoteBuffer: ";
	CoyoteBufferStr += (coyoteTimer_.isActive() ? "Active" : "Inactive");
	Renderer::getInstance().renderText(CoyoteBufferStr, SDL_FRect{ debugInfoRect.x, debugInfoRect.y + 150, debugInfoRect.w, debugInfoRect.h }, debugTextColor, 20);

	Renderer::getInstance().renderRect(getHitBox(), SDL_Color({255, 0, 0, 255})); // 用红色矩形表示玩家碰撞盒
}

void Player::applyGravity(float gravity, float dt) noexcept{
	Physics::gravity(velocityY_, gravity, dt);
}

void Player::reset() noexcept {
	velocityX_ = 0.0f;
	velocityY_ = 0.0f;
	facingRight_ = true;
	isLanded_ = false;
	isJumping_ = false;
	setHitBox(SDL_FRect{ 0, 0, Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT }); // 重置位置到初始状态
}

bool Player::isStateChanged() noexcept{
	PlayerAnimationState nextState = PlayerAnimationState::IDLE;
	if (isLanded_) {
		if (velocityX_ <= 0.1f && velocityX_ >= -0.1f) {
			nextState = PlayerAnimationState::IDLE;
		}
		else 
			nextState = PlayerAnimationState::RUN;
	}
	else if(isJumping_){
		if(velocityY_ < 0.0f) {
			nextState = PlayerAnimationState::JUMP;
		}
		else {
			nextState = PlayerAnimationState::FALL;
		}
	}
	else {
		if (velocityY_ > 0.1f || velocityY_ < -0.1f)
			nextState = PlayerAnimationState::FALL; // 边缘掉落
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
			animationClip.frames.push_back(SDL_FRect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
			break;	
			}
		case PlayerAnimationState::RUN: {
			const auto src = Config::PLAYER_RUN_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(SDL_FRect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			break;
		}
		case PlayerAnimationState::JUMP: {
			const auto src = Config::PLAYER_JUMP_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(SDL_FRect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			animationClip.loop = false; // 跳跃动画不循环，播放完后停在最后一帧，直到状态改变
			break;
		}
		case PlayerAnimationState::FALL: {
			const auto src = Config::PLAYER_FALL_SRC;
			for (const auto& rect : src) {
				animationClip.frames.push_back(SDL_FRect{ static_cast<float>(rect[0]), static_cast<float>(rect[1]), static_cast<float>(rect[2]), static_cast<float>(rect[3]) });
			}
			animationClip.loop = false; // 下降动画不循环，播放完后停在最后一帧，直到状态改变
			break;
		}
		default:{
			const auto src = Config::PLAYER_IDLE_SRC;
			animationClip.frames.push_back(SDL_FRect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
			break;
		}
		}
		animation_.play(animationClip); // 根据当前状态获取对应的动画剪辑并播放
	}
	animation_.update(dt);
}
