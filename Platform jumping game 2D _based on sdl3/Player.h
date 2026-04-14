#pragma once
#include <memory>
#include <cstdint>
#include "Object.h"
#include "render/Animation.h"
#include "render/Camera.h"
#include "core/Timer.h"
#include "core/Vec2.h"
#include "physics/RigidBody.h"

class Resource;
class Renderer;

class Player : public Object {
public:
	struct PlayerCommand {
		bool moveLeft = false;
		bool moveRight = false;
		bool jump = false;
		bool attack = false;
		bool up = false;
		bool down = false;
		bool sprint = false;
		bool flash = false;
	};
	enum class MoveMode : uint8_t { Normal, Climb, Sprint };

	explicit Player(Renderer& renderer, Resource& rM) noexcept;
	~Player() override = default;

	void setCommand(const PlayerCommand& command) noexcept { command_ = command; }

	void update(float dt) noexcept override;
	void render(const Camera& camera) const noexcept override;
	void renderDebug(const Camera& camera) const noexcept;

	float getVelocityX() const noexcept { return rigidBody_.velocity.getX(); }
	float getVelocityY() const noexcept { return rigidBody_.velocity.getY(); }
	bool isLanded() const noexcept { return rigidBody_.isLanded; }
	bool getSprint() const noexcept { return isSprinting_; }
	void reset() noexcept;

	bool isStateChanged() noexcept;
	void updateAnimationState(float dt) noexcept;

	RigidBody& getRigidBody()noexcept { return rigidBody_; }
	bool isAir() const noexcept { return !rigidBody_.isLanded; }
	void judgeClimb(bool canClimb) noexcept { 
		if (moveMode_ == MoveMode::Sprint) {
			return;
		}
		moveMode_ = canClimb ? MoveMode::Climb : MoveMode::Normal; 
	} //update中二次判断是否可以攀爬，根据结果切换移动模式
	void postPhysicsUpdate() noexcept; // 在物理系统更新后调用，处理玩家状态的最终调整，例如根据碰撞结果调整位置、速度等
private:
	void updatePublicStatus(float dt) noexcept; // 更新玩家的公共状态，例如计时器、朝向等，速度积分等物理相关的状态更新放在物理系统中处理，保持Player类的单一职责
	void updateMovementAcceleration(float dt) noexcept; // 更新玩家的加速度，速度积分等物理相关的状态更新放在物理系统中处理
	void finalizeState() noexcept; // 根据当前状态进行最终的状态调整
private:
	Renderer& renderer_;

	bool facingRight_ = true;
	bool isJumping_ = false;
	bool jumpRequested_ = false; // 是否请求跳跃，用于跳跃缓冲
	bool isAttacking_ = false;
	bool attackFacingRight_ = true; // 攻击时的快照
	bool isUP_ = false;
	//bool isDown_ = false;
	bool isClimbing_ = false;
	bool isSprinting_ = false;
	bool wasLanded_ = false; // 上一帧是否着陆，用于检测着陆状态的变化

	PlayerCommand command_ = {};
	PlayerAnimationState currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation animation_;
	std::shared_ptr<SDL_Texture> playerTexture_ = nullptr;
	std::shared_ptr<SDL_Texture> attackTexture_ = nullptr; // 攻击纹理
	Rect attackHitBox_; // 攻击碰撞盒，位置会根据玩家位置和朝向动态调整
	Timer jumpTimer_; // 跳跃缓冲
	Timer coyoteTimer_; // 迟滞时间-土狼时间
	Timer attackTimer_; // 攻击持续时间
	float attackDuration_ = 0.3f; // 攻击持续时间，单位秒
	// 后续添加各方向的攻击

	Vec2 sprintDirection_ = Vec2(0, 0); // 冲刺方向
	Timer sprintTimer_; // 冲刺持续时间
	int sprintMaxCount_ = 2; // 最大冲刺次数，后续增加冲刺冷却时间等功能
	int sprintCount_ = 0; // 当前冲刺次数

	int playerId_ = 0; // 玩家编号

	RigidBody rigidBody_; // 持有一个刚体组件
	MoveMode moveMode_ = MoveMode::Normal; // 当前移动模式 普通 - 攀爬 - 冲刺
	Timer dropDownTimer_; // 平台穿透持续时间，允许玩家在按下下键和跳跃键的组合输入后短暂穿透平台，单位秒
};