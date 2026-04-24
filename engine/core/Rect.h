#pragma once
#include <SDL3/SDL.h>
#include "Vec2.h"

class Rect {
public:
	Rect() noexcept= default;
	Rect(float x, float y, float w, float h) noexcept : m_rect{ x, y, w, h } {}
	~Rect() noexcept = default;

	// 交互
	float x() const noexcept { return m_rect.x; }
	float y() const noexcept { return m_rect.y; }
	float w() const noexcept { return m_rect.w; }
	float h() const noexcept { return m_rect.h; }
	void setX(float x) noexcept { m_rect.x = x; }
	void setY(float y) noexcept { m_rect.y = y; }
	void setW(float w) noexcept { m_rect.w = w; }
	void setH(float h) noexcept { m_rect.h = h; }
	// 功能
	float distance(const Rect& rect) const noexcept;
	bool hasIntersection(const Vec2& point) const noexcept;

	// 隐式转换 引擎内部使用
	operator SDL_FRect() const noexcept { return m_rect; }
private:
	SDL_FRect m_rect = SDL_FRect{};
};