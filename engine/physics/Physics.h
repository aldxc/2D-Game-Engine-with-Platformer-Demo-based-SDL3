#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <cstdint>
#include "RigidBody.h"

class Physics {
public:
	struct physicalCollMap {
		uint8_t size; // 瓦片大小全部相同，正方形瓦片，size表示瓦片的边长，单位为像素
		uint8_t coll; // 碰撞类型
	};
	explicit Physics() = default;
	~Physics() = default;

	// 禁用拷贝与移动
	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;
	Physics(Physics&&) = delete;
	Physics& operator=(Physics&&) = delete;

	bool init(float gravity) noexcept;

	void registerRigidBody(RigidBody& body) noexcept; //注册物理对象
	void unregisterRigidBody(RigidBody& body) noexcept;  // 解绑物理对象

	void update(float dt) noexcept; // 更新物理系统，速度积分
	void resolveCollisions(const std::vector<std::vector<physicalCollMap>>& collmap, float dt, float epsilon) noexcept; // 位移积分，处理碰撞检测和响应

	// AABB碰撞检测函数
	static bool isColliding(const SDL_FRect& a, const SDL_FRect& b) noexcept {
		return SDL_HasRectIntersectionFloat(&a, &b);
	}

	//static void gravity(float& velocityY, float gravity, float dt) noexcept {
	//	velocityY += gravity * dt;
	//}
private:
	std::vector<std::reference_wrapper<RigidBody>> rigidBodies_ = {}; // 物理对象列表
	float gravity_ = 0.0f; // 重力加速度，单位为像素/秒²，后续增加重力缩放等功能
};