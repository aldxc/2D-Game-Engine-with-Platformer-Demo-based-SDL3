#pragma once
#include "core/Vec2.h"
#include "core/Rect.h"

struct RigidBody {
	Vec2 velocity = {}; // 速度向量，表示物体在水平和垂直方向上的速度
	Rect hitBox = {}; // 碰撞盒，表示物体的碰撞范围
	float gravityScale = 1.0f; // 重力缩放因子，控制物体受重力影响的程度，默认为1.0表示正常受重力影响

	Vec2 acceleration = {}; // 加速度向量，玩家通过输入产生的加速度，影响速度的变化
	bool isLanded = false; // 是否着陆，表示玩家是否站在地面上，影响重力和跳跃逻辑
	float maxSpeed = 0.0f; // 最大速度，限制物体的速度不超过这个值

	bool wantsDropDown = false; // 是否希望下落，表示玩家是否希望通过平台下落，影响平台穿透逻辑

	RigidBody() noexcept = default;
	RigidBody(const Vec2& velocity, const Rect& hitBox, float maxSpeed, float gravityScale = 1.0f, const Vec2& acceleration = {}, bool isLanded = false) noexcept
		: velocity(velocity), hitBox(hitBox), maxSpeed(maxSpeed), gravityScale(gravityScale), acceleration(acceleration), isLanded(isLanded) {
	}
	//void applyInput(const Vec2& inputAcceleration, float dt) noexcept {
	//	acceleration = inputAcceleration; // 更新加速度，根据玩家输入调整加速度向量
	//}
};