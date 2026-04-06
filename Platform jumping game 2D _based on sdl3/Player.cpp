#include "Player.h"
#include "render/Renderer.h"
#include "resource/Resource.h"

Player::Player(Animation& animation) : animation_(animation){
	playerTexture_ = Resource::getInstance().loadTexture("resource/characters.png", Renderer::getInstance().getSDLRenderer());
	command_ = {};
	currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation::AnimationClip animationClip;
	const auto src = Config::PLAYER_IDLE_SRC;
	animationClip.frames.push_back(SDL_FRect{ static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), static_cast<float>(src[3]) });
	animation_.play(animationClip);
}

void Player::update(float dt) noexcept{
	//只负责更新速度，位置的更新由外部根据碰撞结果调用applyResolvedMovement来处理，保持Player类的单一职责，后续可以增加状态机等机制来管理玩家状态和行为
	velocityX_ = 0.0f; // 每帧重置水平速度
	if(command_.moveLeft) {
		velocityX_ -= speed_;
		facingRight_ = false;
	}
	if(command_.moveRight) {
		velocityX_ += speed_;
		facingRight_ = true;
	}
	if(command_.jump && isLanded_) {
		velocityY_ = -300.0f; // 跳跃初始速度
		isJumping_ = true;
		isLanded_ = false;
	}
}

void Player::render() const noexcept{

	SDL_FRect debugRect = getHitBox();
	debugRect.w *= 2;
	debugRect.h *= 2;
	debugRect.x -= 16;
	debugRect.y -= 32;
	if (facingRight_)
		Renderer::getInstance().renderTexture(playerTexture_.get(), animation_.getCurrentFrameRect(), debugRect);
	else
		Renderer::getInstance().reversePlayerFaceTexture(playerTexture_.get(), animation_.getCurrentFrameRect(), debugRect);

	Renderer::getInstance().renderRect(getHitBox(), SDL_Color({255, 0, 0, 255})); // 用红色矩形表示玩家
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
		if (velocityX_ == 0.0f) {
			nextState = PlayerAnimationState::IDLE;
		}
		else {
			nextState = PlayerAnimationState::RUN;
		}
	}
	else if(isJumping_){
		if(velocityY_ < 0.0f) {
			nextState = PlayerAnimationState::JUMP;
		}
		else {
			nextState = PlayerAnimationState::FALL;
		}
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
