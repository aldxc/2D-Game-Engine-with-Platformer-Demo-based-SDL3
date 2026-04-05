#include "Player.h"
#include "render/Renderer.h"

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
	Renderer::getInstance().renderRect(getHitBox(), SDL_Color({ 255, 0, 0, 255 })); // 用红色矩形表示玩家
}

void Player::reset() noexcept {
	velocityX_ = 0.0f;
	velocityY_ = 0.0f;
	facingRight_ = true;
	isLanded_ = false;
	isJumping_ = false;
	setHitBox(SDL_FRect{ 0, 0, Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT }); // 重置位置到初始状态
}