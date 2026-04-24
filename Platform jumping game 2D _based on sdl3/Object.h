#pragma once

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
	//camera参数用于将对象的世界坐标转换为屏幕坐标进行渲染
	virtual void render(const Camera& camera) const noexcept = 0; 
	bool getIsActive() const noexcept { return m_isActive; }
	bool getIsDestroyed() const noexcept { return m_isDestroyed; }
	bool getIsCollidable() const noexcept { return m_isCollidable; }
protected:
	void setIsActive(bool active) noexcept { m_isActive = active; }
	void setIsDestroyed(bool destroyed) noexcept { m_isDestroyed = destroyed; }
	void setIsCollidable(bool coll) noexcept { m_isCollidable = coll; }
	void reset() noexcept {
		m_isActive = true;
		m_isDestroyed = false;
		m_isCollidable = true;
	}
private:

	bool m_isActive = true;	   // 是否活跃，控制对象的更新和渲染
	bool m_isDestroyed = false; // 是否被销毁，控制对象的生命周期
	bool m_isCollidable = true; // 是否可碰撞，控制碰撞检测 // 这里暂时不区分不可碰撞和被销毁的对象
};