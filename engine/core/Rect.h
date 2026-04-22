#pragma once
#include <SDL3/SDL.h>
#include "Vec2.h"

class Rect {
public:
	Rect() noexcept= default;
	Rect(float x, float y, float w, float h) noexcept : rect_{ x, y, w, h } {}
	~Rect() noexcept = default;

	// 交互
	float x() const noexcept { return rect_.x; }
	float y() const noexcept { return rect_.y; }
	float w() const noexcept { return rect_.w; }
	float h() const noexcept { return rect_.h; }
	void setX(float x) noexcept { rect_.x = x; }
	void setY(float y) noexcept { rect_.y = y; }
	void setW(float w) noexcept { rect_.w = w; }
	void setH(float h) noexcept { rect_.h = h; }
	float distance(const Rect& rect) const noexcept;
	bool hasIntersection(const Vec2& point) const noexcept;

	// 隐式转换 引擎内部使用
	operator SDL_FRect() const noexcept { return rect_; }
	//operator const SDL_FRect& () const noexcept { return rect_; }
private:
	SDL_FRect rect_ = SDL_FRect{};
};