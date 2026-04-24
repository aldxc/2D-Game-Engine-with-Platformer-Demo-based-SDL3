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

	void setCommand(const PlayerCommand& command) noexcept { m_command = command; }

	// 更新玩家状态，包括位置、动画状态、攻击状态等，dt为帧间时间，单位秒
	void update(double dt) noexcept override;

	void render(const Camera& camera) const noexcept override;
	void renderDebug(const Camera& camera) const noexcept;
	bool isStateChanged() noexcept;
	void updateAnimationState(double dt) noexcept;
	void postPhysicsUpdate() noexcept;

	// 获取玩家状态
	float getVelocityX() const noexcept { return m_rigidBody.velocity.x(); }
	float getVelocityY() const noexcept { return m_rigidBody.velocity.y(); }
	bool isLanded() const noexcept { return m_rigidBody.isLanded; }
	bool getSprint() const noexcept { return m_isSprinting; }
	RigidBody& getRigidBody()noexcept { return m_rigidBody; }
	bool isAir() const noexcept { return !m_rigidBody.isLanded; }
	bool isAttacking() const noexcept { return m_isAttacking; }
	Rect getAttackHitBox() const noexcept { return m_attackHitBox; }
	void dead() noexcept { m_isDead = true; }
	bool isDead() const noexcept { return m_isDead; }
	bool getWin() const noexcept { return m_isWinner; }
	std::vector<SfxId>& getSfxToPlay() noexcept { return m_sfxToPlay; }

	// 设置玩家状态
	void judgeClimb(bool canClimb) noexcept;
	void setHit() noexcept;
	void setFaced(bool faced) noexcept { m_facingRight = faced; }

	void setBirthPoint(const Rect& birthPoint) noexcept {
		m_birthPoint.setX(birthPoint.x());
		m_birthPoint.setY(birthPoint.y());
	}

	void setPlayerID(int id) noexcept { m_playerId = id; }
	void setWin() noexcept { m_isWinner = true; setIsCollidable(false); }

private:
	// 更新玩家的公共状态
	void updatePublicStatus(double dt) noexcept;

	// 更新玩家的加速度
	void updateMovementAcceleration(double dt) noexcept;

	// 根据当前状态进行最终的状态调整
	void finalizeState() noexcept;

private:
	// 外部依赖
	Renderer& m_renderer;

	// 资源
	std::shared_ptr<SDL_Texture> m_playerTexture = nullptr;
	std::shared_ptr<SDL_Texture> m_attackTexture = nullptr;

	// 出生点坐标，玩家重置时会回到这个位置
	Vec2 m_birthPoint = Vec2(0, 0);

	// 输入与动画
	PlayerCommand m_command = {};
	PlayerAnimationState m_currentAnimationState = PlayerAnimationState::IDLE;
	Animation m_animation;

	// 朝向与基础状态
	bool m_facingRight = true;
	bool m_isJumping = false;
	bool m_jumpRequested = false;
	bool m_isAttacking = false;
	bool m_attackFacingRight = true;
	bool m_isUP = false;
	bool m_isDown = false;
	bool m_isClimbing = false;

	// 有 bug，冲刺时可以改变方向，后续看是否需要修改还是保存当作机制
	bool m_isSprinting = false;

	bool m_wasLanded = false;
	bool m_isHited = false;
	bool m_isDead = false;
	bool m_isWinner = false;

	// 碰撞与运动相关
	Rect m_attackHitBox;
	Vec2 m_sprintDirection = Vec2(0, 0);
	RigidBody m_rigidBody;
	MoveMode m_moveMode = MoveMode::NORMAL;

	// 计时器
	// 受击后无敌时间，单位秒
	Timer m_hitTimer;

	Timer m_jumpTimer;

	// 迟滞时间 - 土狼时间
	Timer m_coyoteTimer;

	// 攻击持续时间
	Timer m_attackTimer;

	// 冲刺持续时间
	Timer m_sprintTimer;

	// 平台穿透持续时间，允许玩家在按下下键和跳跃键的组合输入后短暂穿透平台，单位秒
	Timer m_dropDownTimer;

	// 数值配置
	float m_attackDuration = 0.3f;

	// 最大冲刺次数，后续增加冲刺冷却时间等功能
	int32_t m_sprintMaxCount = 2;

	// 当前冲刺次数
	int32_t m_sprintCount = 0;

	// 玩家编号
	size_t m_playerId = 0;

	// 玩家生命值
	int32_t m_hp = Config::PLAYER_HP;

	// 玩家最大生命值
	int32_t m_maxHp = Config::PLAYER_HP;

	// 在当前帧需要播放的音效列表
	std::vector<SfxId> m_sfxToPlay;
};