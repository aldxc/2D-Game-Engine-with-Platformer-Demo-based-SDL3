#pragma once
#include "core/Vec2.h"
#include "core/Rect.h"

struct RigidBody {
	// 速度向量
	Vec2 velocity = {}; 
	// 碰撞盒
	Rect hitBox = {}; 
	// 重力缩放因子
	float gravityScale = 1.0f; 
	// 加速度向量
	Vec2 acceleration = {}; 
	// 是否着陆
	bool isLanded = false; 
	// 最大水平速度
	float maxSpeed = 0.0f; 
	// 最大下落速度
	float maxFallSpeed = 0.0f; 
	// 玩家是否希望穿过半平台
	bool wantsDropDown = false; 

	RigidBody() noexcept = default;
	RigidBody(const Vec2& velocity, const Rect& hitBox, float maxSpeed, float gravityScale = 1.0f, const Vec2& acceleration = {}, bool isLanded = false) noexcept
		: velocity(velocity), hitBox(hitBox), maxSpeed(maxSpeed), gravityScale(gravityScale), acceleration(acceleration), isLanded(isLanded) {
	}
};