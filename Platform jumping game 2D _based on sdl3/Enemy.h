#pragma once
#include <memory>
#include <vector>
#include "Object.h"
#include "physics/RigidBody.h"
#include "render/Animation.h"
#include "core/Timer.h"
#include "AudioId.h"

class Renderer;
class Resource;
class Rect;
struct physicalCollMap;

// 后续加入状态机
class Enemy : public Object{
public:
	explicit Enemy(Renderer& renderer, Resource& rM) noexcept;
	~Enemy() override = default;

	void init(Rect enemyInfo) noexcept; // 初始化敌人状态，例如位置、动画状态等，后续增加更多的初始化功能如AI状态等

	void update(double dt) noexcept override;

	void render(const Camera& camera) const noexcept override;

	void renderDebug(const Camera& camera) const noexcept; // 用于渲染敌人的调试信息，例如碰撞盒等，后续增加更多的调试信息如状态、路径等

	void reset(Rect hitbox = Rect(0, 0, 0, 0)) noexcept; // 重置敌人状态，例如位置、动画状态等，后续增加更多的重置功能如重置AI状态等

	void setFacingRight(const Rect& player, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept; // AI逻辑判断敌人朝向，同时结合地图碰撞信息调整朝向，防止敌人卡在墙壁或掉落

	void updateAnimationState(double dt) noexcept; // 根据当前状态更新动画状态

	RigidBody& getRigidBody() noexcept { return rigidBody_; }
	bool getRegisteredToPhysics() const noexcept { return isRegisteredToPhysics_; }
	void setRegisteredToPhysics(bool registered) noexcept { isRegisteredToPhysics_ = registered; }
	void kill() noexcept; // 使敌人死亡
	bool IsDead() const noexcept { return !isAlive_; } // 正常被攻击时判定
	void takeHit(int dir) noexcept; // 使敌人受伤，后续增加受伤逻辑时使用
	std::vector<SfxId>& getSfxToPlay() noexcept { return sfxToplay_; }
private:
	bool isStateChanged() noexcept; // 判断敌人状态是否发生变化
private:
	Renderer& renderer_;

	RigidBody rigidBody_;
	bool isRegisteredToPhysics_ = false; // 是否已经注册到物理引擎，控制敌人状态重置时的物理引擎注册逻辑

	PlayerAnimationState currentAnimationState_ = PlayerAnimationState::IDLE;
	Animation animation_;
	bool isFacingRight_ = true;
	bool isTrackingPlayer_ = false; // 是否正在跟踪玩家
	bool isAlreadyTracking_ = false; // 是否已经到达位置，防止敌人跟踪到后抖动
	bool isAlive_ = true; // 是否处于活跃状态
	bool isHited_ = false; // 是否处于受伤状态

	std::shared_ptr<SDL_Texture> enemyTexture_ = nullptr;
	int32_t hp_ = Config::ENEMY_HP; // 敌人生命值，后续增加敌人受伤逻辑时使用
	int32_t maxHp_ = Config::ENEMY_HP; // 敌人最大生命值，后续增加敌人受伤逻辑时使用

	Timer trackingTimer_; // 悬崖检测计时器，控制敌人检测到悬崖时的回头频率，防止频繁回头导致的抖动
	Timer deathTimer_; // 死亡定时器，控制敌人死亡后的消失时间
	//Timer hitTimer_; // 
	Timer hitInvincibleTimer_; // 受击无敌事件（与攻击时间相同，防止一次攻击多次触发受伤逻辑）

	std::vector<SfxId> sfxToplay_;
};