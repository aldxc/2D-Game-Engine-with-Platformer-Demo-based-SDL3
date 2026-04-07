#pragma once
#include <SDL3/SDL.h>

class Physics {
public:
	explicit Physics() = default;
	~Physics() = default;

	// 禁用拷贝与移动
	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;
	Physics(Physics&&) = delete;
	Physics& operator=(Physics&&) = delete;

	bool init() noexcept;
	
	//struct PhysicsResult { //碰撞结果
	//	SDL_FRect rect;// 碰撞后的碰撞盒位置和大小
	//	float velocityX; // 碰撞后的水平速度
	//	float velocityY; // 碰撞后的垂直速度
	//	bool hitLeft; // 是否碰撞到左侧
	//	bool hitRight; // 是否碰撞到右侧
	//	bool hitTop; // 是否碰撞到顶部
	//	bool hitBottom; // 是否碰撞到底部
	//};

	// AABB碰撞检测函数
	static bool isColliding(const SDL_FRect& a, const SDL_FRect& b) noexcept {
		return SDL_HasRectIntersectionFloat(&a, &b);
	}

	static void gravity(float& velocityY, float gravity, float dt) noexcept {
		velocityY += gravity * dt; // 根据重力加速度和时间增量更新垂直速度
	}
};