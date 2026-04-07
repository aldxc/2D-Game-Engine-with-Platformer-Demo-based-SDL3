#pragma once
#include <memory>
#include "Object.h"
#include "render/Animation.h"


class Player : public Object {
public:
	struct PlayerCommand {
		bool moveLeft = false;
		bool moveRight = false;
		bool jump = false;
	};

	//explicit Player() = default;
	explicit Player();
	~Player() override = default;

	void setCommand(const PlayerCommand& command) noexcept { command_ = command; }

	void update(float dt) noexcept override;
	void render() const noexcept override;
	void renderDebug() const noexcept;

	float getVelocityX() const noexcept { return velocityX_; }
	float getVelocityY() const noexcept { return velocityY_; }
	bool isLanded() const noexcept { return isLanded_; }

	void setVelocityX(float velocityX) noexcept { velocityX_ = velocityX; }
	void setVelocityY(float velocityY) noexcept { velocityY_ = velocityY; }
	void setLanded(bool landed) noexcept { isLanded_ = landed; }

	void applyGravity(float gravity, float dt) noexcept;

	void applyResolvedMovement(const SDL_FRect& hitBox, float velocityX, float velocityY, bool landed) noexcept {
		setHitBox(hitBox);
		velocityX_ = velocityX;
		velocityY_ = velocityY;
		isLanded_ = landed;
		if (landed) {
			isJumping_ = false;
		}
	}
	void reset() noexcept;

	bool isStateChanged() noexcept;
	void updateAnimationState(float dt) noexcept;
private:
	float speed_ = 200.0f;
	float velocityX_ = 0.0f;
	float velocityY_ = 0.0f;
	bool facingRight_ = true;
	bool isLanded_ = false;
	bool isJumping_ = false;

	PlayerCommand command_ = {};
	PlayerAnimationState currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation animation_;
	std::shared_ptr<SDL_Texture> playerTexture_ = nullptr;
};