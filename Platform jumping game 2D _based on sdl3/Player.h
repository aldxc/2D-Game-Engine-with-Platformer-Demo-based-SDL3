#pragma once
#include <memory>
#include "Object.h"
#include "render/Animation.h"
#include "render/Camera.h"
#include "core/Timer.h"


class Player : public Object {
public:
	struct PlayerCommand {
		bool moveLeft = false;
		bool moveRight = false;
		bool jump = false;
		bool attack = false;
		bool climb = false;
		bool sprint = false;
		bool flash = false;
	};

	explicit Player();
	~Player() override = default;

	void setCommand(const PlayerCommand& command) noexcept { command_ = command; }

	void update(float dt) noexcept override;
	void render(const Camera& camera) const noexcept override;
	void renderDebug(const Camera& camera) const noexcept;

	float getVelocityX() const noexcept { return velocityX_; }
	float getVelocityY() const noexcept { return velocityY_; }
	bool isLanded() const noexcept { return isLanded_; }

	void setVelocityX(float velocityX) noexcept { velocityX_ = velocityX; }
	void setVelocityY(float velocityY) noexcept { velocityY_ = velocityY; }
	void setLanded(bool landed) noexcept { isLanded_ = landed; }

	void applyGravity(float gravity, float dt) noexcept;

	void applyResolvedMovement(const SDL_FRect& hitBox, float velocityX, float velocityY, bool landed) noexcept;
	void reset() noexcept;

	bool isStateChanged() noexcept;
	void updateAnimationState(float dt) noexcept;
private:
	float speed_ = 0.0f;
	float velocityX_ = 0.0f;
	float velocityY_ = 0.0f;
	bool facingRight_ = true;
	bool isLanded_ = false;
	bool isJumping_ = false;
	bool isAttacking_ = false;
	bool attackFacingRight_ = true; // 攻击时的快照

	PlayerCommand command_ = {};
	PlayerAnimationState currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation animation_;
	std::shared_ptr<SDL_Texture> playerTexture_ = nullptr;
	std::shared_ptr<SDL_Texture> attackTexture_ = nullptr; // 攻击纹理
	SDL_FRect attackHitBox_; // 攻击碰撞盒，位置会根据玩家位置和朝向动态调整
	Timer jumpTimer_; // 跳跃缓冲
	Timer coyoteTimer_; // 迟滞时间-土狼时间
	Timer attackTimer_; // 攻击持续时间
	float attackDuration_ = 0.3f; // 攻击持续时间，单位秒

	int playerId_ = 0; // 玩家编号
};