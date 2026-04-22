#pragma once
#include <memory>
#include <cstdint>
#include <vector>
#include "Object.h"
#include "render/Animation.h"
#include "core/Timer.h"
#include "core/Vec2.h"
#include "physics/RigidBody.h"
#include "Config.h"
#include "AudioId.h"

// 前向声明，减少头文件依赖
class Resource;
class Camera;
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

	enum class MoveMode : uint8_t { NORMAL, CLIMB, SPRINT };

	explicit Player(Renderer& renderer, Resource& rM) noexcept;
	~Player() override = default;

	void reset() noexcept;

	void setCommand(const PlayerCommand& command) noexcept { command_ = command; }

	// 更新玩家状态，包括位置、动画状态、攻击状态等，dt为帧间时间，单位秒
	void update(double dt) noexcept override;

	void render(const Camera& camera) const noexcept override;
	void renderDebug(const Camera& camera) const noexcept;
	bool isStateChanged() noexcept;
	void updateAnimationState(double dt) noexcept;
	void postPhysicsUpdate() noexcept;

	// 获取玩家状态
	float getVelocityX() const noexcept { return rigidBody_.velocity.x(); }
	float getVelocityY() const noexcept { return rigidBody_.velocity.y(); }
	bool isLanded() const noexcept { return rigidBody_.isLanded; }
	bool getSprint() const noexcept { return isSprinting_; }
	RigidBody& getRigidBody()noexcept { return rigidBody_; }
	bool isAir() const noexcept { return !rigidBody_.isLanded; }
	bool isAttacking() const noexcept { return isAttacking_; }
	Rect getAttackHitBox() const noexcept { return attackHitBox_; }
	void dead() noexcept { isDead_ = true; }
	bool isDead() const noexcept { return isDead_; }
	bool getWin() const noexcept { return isWinner_; }
	std::vector<SfxId>& getSfxToPlay() noexcept { return sfxToPlay_; }

	// 设置玩家状态
	void judgeClimb(bool canClimb) noexcept;
	void setHit() noexcept;
	void setFaced(bool faced) noexcept { facingRight_ = faced; }

	void setBirthPoint(const Rect& birthPoint) noexcept {
		birthPoint_.setX(birthPoint.x());
		birthPoint_.setY(birthPoint.y());
	}

	void setPlayerID(int id) noexcept { playerId_ = id; }
	void setWin() noexcept { isWinner_ = true; setIsCollidable(false); }

private:
	// 更新玩家的公共状态
	void updatePublicStatus(double dt) noexcept;

	// 更新玩家的加速度
	void updateMovementAcceleration(double dt) noexcept;

	// 根据当前状态进行最终的状态调整
	void finalizeState() noexcept;

private:
	// 外部依赖
	Renderer& renderer_;

	// 资源
	std::shared_ptr<SDL_Texture> playerTexture_ = nullptr;
	std::shared_ptr<SDL_Texture> attackTexture_ = nullptr;

	// 出生点坐标，玩家重置时会回到这个位置
	Vec2 birthPoint_ = Vec2(0, 0);

	// 输入与动画
	PlayerCommand command_ = {};
	PlayerAnimationState currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation animation_;

	// 朝向与基础状态
	bool facingRight_ = true;
	bool isJumping_ = false;
	bool jumpRequested_ = false;
	bool isAttacking_ = false;
	bool attackFacingRight_ = true;
	bool isUP_ = false;
	bool isDown_ = false;
	bool isClimbing_ = false;

	// 有 bug，冲刺时可以改变方向，后续看是否需要修改还是保存当作机制
	bool isSprinting_ = false;

	bool wasLanded_ = false;
	bool isHited_ = false;
	bool isDead_ = false;
	bool isWinner_ = false;

	// 碰撞与运动相关
	Rect attackHitBox_;
	Vec2 sprintDirection_ = Vec2(0, 0);
	RigidBody rigidBody_;
	MoveMode moveMode_ = MoveMode::NORMAL;

	// 计时器
	// 受击后无敌时间，单位秒
	Timer hitTimer_;

	Timer jumpTimer_;

	// 迟滞时间 - 土狼时间
	Timer coyoteTimer_;

	// 攻击持续时间
	Timer attackTimer_;

	// 冲刺持续时间
	Timer sprintTimer_;

	// 平台穿透持续时间，允许玩家在按下下键和跳跃键的组合输入后短暂穿透平台，单位秒
	Timer dropDownTimer_;

	// 数值配置
	float attackDuration_ = 0.3f;

	// 最大冲刺次数，后续增加冲刺冷却时间等功能
	int32_t sprintMaxCount_ = 2;

	// 当前冲刺次数
	int32_t sprintCount_ = 0;

	// 玩家编号
	size_t playerId_ = 0;

	// 玩家生命值
	int32_t hp_ = Config::PLAYER_HP;

	// 玩家最大生命值
	int32_t maxHp_ = Config::PLAYER_HP;

	// 在当前帧需要播放的音效列表
	std::vector<SfxId> sfxToPlay_;
};