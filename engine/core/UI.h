#pragma once

// UI基类，与state类似，使用CRTP模式实现UI类型的静态多态，提供输入处理、更新和渲染接口，以及UI类型比较和获取接口
template<class TUIType>
class UI {
public:
	explicit UI(TUIType type) noexcept : m_type(type) {}
	virtual ~UI() = default;

	UI(const UI&) = delete;
	UI& operator=(const UI&) = delete;
	UI(UI&&) = delete;
	UI& operator=(UI&&) = delete;

	virtual void handleInput() noexcept = 0;
	virtual void update(double dt) noexcept = 0;
	virtual void render() const noexcept = 0;

	bool isType(TUIType type) const noexcept { return m_type == type; }
	TUIType getType() const noexcept { return m_type; }

private:
	TUIType m_type;
};