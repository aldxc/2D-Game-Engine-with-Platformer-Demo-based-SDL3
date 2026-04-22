#pragma once
#include <memory>
#include <cstdint>
#include <vector>
#include <functional>
#include "Object.h"
#include "render/Animation.h"
#include "core/Timer.h"
#include "core/Vec2.h"
#include "physics/RigidBody.h"
#include "AudioId.h"

class Resource;
class Camera;
class Renderer;
class EventManager;

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

	void update(double dt) noexcept override;
	void render(const Camera& camera) const noexcept override;
	void renderDebug(const Camera& camera) const noexcept;

	float getVelocityX() const noexcept { return rigidBody_.velocity.getX(); }
	float getVelocityY() const noexcept { return rigidBody_.velocity.getY(); }
	bool isLanded() const noexcept { return rigidBody_.isLanded; }
	bool getSprint() const noexcept { return isSprinting_; }
	void reset() noexcept;

	bool isStateChanged() noexcept;
	void updateAnimationState(double dt) noexcept;

	RigidBody& getRigidBody()noexcept { return rigidBody_; }
	bool isAir() const noexcept { return !rigidBody_.isLanded; }
	bool isAttacking() const noexcept { return isAttacking_; }
	Rect getAttackHitBox() const noexcept { return attackHitBox_; }
	void judgeClimb(bool canClimb) noexcept; //update中二次判断是否可以攀爬，根据结果切换移动模式
	void postPhysicsUpdate() noexcept; // 在物理系统更新后调用，处理玩家状态的最终调整，例如根据碰撞结果调整位置、速度等
	void setHit() noexcept;
	void setFaced(bool faced) noexcept { facingRight_ = faced; }
	void setBirthPoint(const Rect& birthPoint) noexcept { 
		birthPoint_.setX(birthPoint.x());
		birthPoint_.setY(birthPoint.y());
	}
	void dead() noexcept { isDead_ = true; }// 处理玩家死亡逻辑
	bool isDead() const noexcept { return isDead_; }
	void setPlayerID(int id) noexcept { playerId_ = id; }
	std::vector<SfxId>& getSfxToPlay() noexcept { return sfxToPlay_; } // 获取当前帧需要播放的音效列表，供外部系统调用后清空
	void setWin() noexcept { isWinner_ = true; setIsCollidable(false); }
	bool getWin() const noexcept { return isWinner_; }
private:
	void updatePublicStatus(double dt) noexcept; // 更新玩家的公共状态，例如计时器、朝向等，速度积分等物理相关的状态更新放在物理系统中处理，保持Player类的单一职责
	void updateMovementAcceleration(double dt) noexcept; // 更新玩家的加速度，速度积分等物理相关的状态更新放在物理系统中处理
	void finalizeState() noexcept; // 根据当前状态进行最终的状态调整
private:
	Renderer& renderer_;

	Vec2 birthPoint_ = Vec2(0, 0); // 出生点坐标，玩家重置时会回到这个位置

	bool facingRight_ = true;
	bool isJumping_ = false;
	bool jumpRequested_ = false; // 是否请求跳跃，用于跳跃缓冲
	bool isAttacking_ = false;
	bool attackFacingRight_ = true; // 攻击时的快照
	bool isUP_ = false;
	bool isDown_ = false;
	bool isClimbing_ = false;
	bool isSprinting_ = false; // 有bug，冲刺时可以改变方向，后续看是否需要修改还是保存当作机制
	bool wasLanded_ = false; // 上一帧是否着陆，用于检测着陆状态的变化
	bool isHited_ = false; // 受到攻击

	Timer hitTimer_; // 受击后无敌时间，单位秒

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
	int32_t sprintMaxCount_ = 2; // 最大冲刺次数，后续增加冲刺冷却时间等功能
	int32_t sprintCount_ = 0; // 当前冲刺次数

	size_t playerId_ = 0; // 玩家编号

	RigidBody rigidBody_; // 持有一个刚体组件
	MoveMode moveMode_ = MoveMode::Normal; // 当前移动模式 普通 - 攀爬 - 冲刺
	Timer dropDownTimer_; // 平台穿透持续时间，允许玩家在按下下键和跳跃键的组合输入后短暂穿透平台，单位秒
	bool isDead_ = false;
	int32_t hp_ = Config::PLAYER_HP; // 玩家生命值
	int32_t maxHp_ = Config::PLAYER_HP; // 玩家最大生命值

	std::vector<SfxId> sfxToPlay_; // 在当前帧需要播放的音效列表
	bool isWinner_ = false; // 是否获胜
};