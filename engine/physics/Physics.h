#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <cstdint>
#include "RigidBody.h"

struct physicalCollMap;

class Physics {
public:

	explicit Physics() = default;
	~Physics() = default;

	// 禁用拷贝与移动
	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;
	Physics(Physics&&) = delete;
	Physics& operator=(Physics&&) = delete;

	bool init(float gravity) noexcept;

	//注册、解绑物理对象
	void registerRigidBody(RigidBody& body) noexcept; 
	void unregisterRigidBody(RigidBody& body) noexcept; 

	// 更新物理系统，速度积分
	void update(double dt) noexcept; 

	// 位移积分，处理碰撞检测和响应
	void resolveCollisions(const std::vector<std::vector<physicalCollMap>>& collmap, double dt, float epsilon) noexcept; 

	// AABB碰撞检测函数
	static bool isColliding(const SDL_FRect& a, const SDL_FRect& b) noexcept {
		return SDL_HasRectIntersectionFloat(&a, &b);
	}

	// 判断a和b之间是否有视线，考虑地图碰撞信息
	static bool LineOfSight(const SDL_FRect& a, const SDL_FRect& b, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept; 

	// 判断玩家前方是否有地面，考虑地图碰撞信息，facingRight表示玩家朝向，hitBox表示玩家的碰撞盒
	static bool hasGroundAhead(const SDL_FRect& hitBox, bool facingRight, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept;

	// 判断玩家前方是否有墙，考虑地图碰撞信息，facingRight表示玩家朝向，hitBox表示玩家的碰撞盒
	static bool hasWallAhead(const SDL_FRect& hitBox, bool facingRight, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept;
private:
	// 物理对象列表
	std::vector<std::reference_wrapper<RigidBody>> m_rigidBodies = {}; 
	// 重力加速度，单位为像素/秒²
	float m_gravity = 0.0f; 
};