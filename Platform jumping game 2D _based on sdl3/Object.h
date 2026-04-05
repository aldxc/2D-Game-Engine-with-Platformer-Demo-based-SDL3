#pragma once
#include <SDL3/SDL.h>
#include "Config.h"

//游戏对象基类
class Object {
public:
	explicit Object() = default;
	virtual ~Object() = default;

	//禁用拷贝与移动
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	Object(Object&&) = delete;
	Object& operator=(Object&&) = delete;

	virtual void update(float dt) noexcept = 0;
	virtual void render() const noexcept = 0;
	//获取对象位置和大小接口
	const SDL_FRect& getHitBox() const noexcept { return hitBox_; }
protected:
	void setMoveXY(float dx, float dy) noexcept {	// 移动对象位置接口，更新碰撞盒位置
		hitBox_.x += dx;
		hitBox_.y += dy;
	}
	void setHitBox(const SDL_FRect& rect) noexcept { hitBox_ = rect; } // 设置碰撞盒位置和大小接口
private:
	SDL_FRect hitBox_ = SDL_FRect{ 0,0, Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT }; // 碰撞盒 同时也可以用来表示对象的位置和大小

	bool isActive_ = true; // 是否活跃，控制对象的更新和渲染
	bool isDestroyed_ = false; // 是否被销毁，控制对象的生命周期
	bool isCollidable_ = true; // 是否可碰撞，控制碰撞检测
};