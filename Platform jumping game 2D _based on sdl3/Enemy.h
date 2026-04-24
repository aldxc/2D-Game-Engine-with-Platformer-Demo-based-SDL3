#pragma once
#include <memory>
#include <vector>
#include "Object.h"
#include "physics/RigidBody.h"
#include "render/Animation.h"
#include "core/Timer.h"
#include "Config.h"
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

	void init(Rect enemyInfo) noexcept; 

	void update(double dt) noexcept override;

	void render(const Camera& camera) const noexcept override;

	void renderDebug(const Camera& camera) const noexcept; 

	void reset(Rect hitbox = Rect(0, 0, 0, 0)) noexcept; 
	// AI逻辑判断敌人朝向，同时结合地图碰撞信息调整朝向，防止敌人卡在墙壁或掉落
	void setFacingRight(const Rect& player, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept; 

	void updateAnimationState(double dt) noexcept; // 根据当前状态更新动画状态

	RigidBody& getRigidBody() noexcept { return m_rigidBody; }
	bool getRegisteredToPhysics() const noexcept { return m_isRegisteredToPhysics; }
	void setRegisteredToPhysics(bool registered) noexcept { m_isRegisteredToPhysics = registered; }
	void kill() noexcept; 
	bool IsDead() const noexcept { return !m_isAlive; } 
	void takeHit(int dir) noexcept; 
	std::vector<SfxId>& getSfxToPlay() noexcept { return m_sfxToplay; }
private:
	// 判断敌人状态是否发生变化
	bool isStateChanged() noexcept; 
private:
	Renderer& m_renderer;

	RigidBody m_rigidBody;
	// 是否已经注册到物理引擎，控制敌人状态重置时的物理引擎注册逻辑
	bool m_isRegisteredToPhysics = false; 

	PlayerAnimationState m_currentAnimationState = PlayerAnimationState::IDLE;
	Animation m_animation;
	bool m_isFacingRight = true;
	bool m_isTrackingPlayer = false; // 是否正在跟踪玩家
	bool m_isAlreadyTracking = false; // 是否已经到达位置，防止敌人跟踪到后抖动
	bool m_isAlive = true; 
	bool m_isHited = false; 

	std::shared_ptr<SDL_Texture> m_enemyTexture = nullptr;
	int32_t m_hp = Config::ENEMY_HP; 
	size_t m_maxHp = Config::ENEMY_HP; 
	// 悬崖检测计时器，控制敌人检测到悬崖时的回头频率，防止频繁回头导致的抖动
	Timer m_trackingTimer; 
	// 死亡定时器，控制敌人死亡后的消失时间
	Timer m_deathTimer; 
	// 受击无敌事件（与攻击时间相同，防止一次攻击多次触发受伤逻辑）
	Timer m_hitInvincibleTimer; 

	// 当前帧需要播放的音效列表，供外部系统调用后清空
	std::vector<SfxId> m_sfxToplay;
};