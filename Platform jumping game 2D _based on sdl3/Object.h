#pragma once
#include <SDL3/SDL.h>
#include "Config.h"

class Camera;

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

	virtual void update(double dt) noexcept = 0;
	virtual void render(const Camera& camera) const noexcept = 0; //camera参数用于将对象的世界坐标转换为屏幕坐标进行渲染
	bool getIsActive() const noexcept { return isActive_; }
	bool getIsDestroyed() const noexcept { return isDestroyed_; }
	bool getIsCollidable() const noexcept { return isCollidable_; }
	//bool getIsCollidable() const noexcept { return isCollidable_; }
protected:
	void setIsActive(bool active) noexcept { isActive_ = active; }
	void setIsDestroyed(bool destroyed) noexcept { isDestroyed_ = destroyed; }
	void setIsCollidable(bool coll) noexcept { isCollidable_ = coll; }
	void reset() noexcept {
		isActive_ = true;
		isDestroyed_ = false;
		isCollidable_ = true;
	}
private:

	bool isActive_ = true; // 是否活跃，控制对象的更新和渲染
	bool isDestroyed_ = false; // 是否被销毁，控制对象的生命周期
	bool isCollidable_ = true; // 是否可碰撞，控制碰撞检测 // 这里暂时不区分不可碰撞和被销毁的对象
};