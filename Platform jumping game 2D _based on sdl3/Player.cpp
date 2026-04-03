#include "Player.h"
#include "render/Renderer.h"

void Player::update(float dt) noexcept{
	velocityX_ = 0.0f;
	velocityY_ = 0.0f;
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
	setMoveXY(velocityX_ * dt, velocityY_ * dt); // 根据速度和时间更新位置

	//后续增加重力、碰撞检测等逻辑
	//player状态切换逻辑，如从跳跃状态切换到着陆状态等
}

void Player::render() const noexcept{
	Renderer::getInstance().renderRect(getHitBox(), SDL_Color({ 255, 0, 0, 255 })); // 用红色矩形表示玩家
}
