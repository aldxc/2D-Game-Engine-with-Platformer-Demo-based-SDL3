#include <algorithm>
#include "Camera.h"

void Camera::setViewport(float x, float y, float width, float height, float zoom) noexcept {
	viewX_ = x;
	viewY_ = y;
	viewWidth_ = width;
	viewHeight_ = height;
	zoom_ = zoom;
	deadZone_ = { viewWidth_ * 0.25f, viewHeight_ * 0.25f, viewWidth_ * 0.5f, viewHeight_ * 0.5f }; // 死区设置为视口中心的50%区域，后续增加自定义死区设置功能
}

void Camera::setPosition(float x, float y) noexcept {
	viewX_ = x;
	viewY_ = y;
}

void Camera::setWorldBounds(float x, float y, float width, float height) noexcept{
	worldBounds_ = { x, y, width, height };
	useWorldBounds_ = true;
}

void Camera::followTarget(const SDL_FRect& target, float dt, bool smooth, float followSpeed) noexcept{
	//clampToBounds();// 在跟随目标后限制摄像机位置在世界边界内

	if(target.x + target.w > viewX_ + deadZone_.x + deadZone_.w) {
		float targetX = target.x + target.w - (viewX_ + deadZone_.x + deadZone_.w);
		viewX_ += smooth ? (targetX * followSpeed * dt) : targetX;
	} else if(target.x < viewX_ + deadZone_.x) {
		float targetX = target.x - (viewX_ + deadZone_.x);
		viewX_ += smooth ? (targetX * followSpeed * dt) : targetX;
	}

	clampToBounds();// 在跟随目标后限制摄像机位置在世界边界内
}

void Camera::clampToBounds() noexcept{
	if(useWorldBounds_) {
		viewX_ = std::clamp(viewX_, worldBounds_.x, worldBounds_.x + worldBounds_.w - viewWidth_);
		viewY_ = std::clamp(viewY_, worldBounds_.y, worldBounds_.y + worldBounds_.h - viewHeight_);
	}
}

Rect Camera::worldToScreen(const SDL_FRect& worldRect) const noexcept{
	Rect screenRect = {};
	screenRect.setX((worldRect.x - viewX_) * zoom_);
	screenRect.setY((worldRect.y - viewY_) * zoom_);
	screenRect.setW(worldRect.w * zoom_);
	screenRect.setH(worldRect.h * zoom_);

	return screenRect;
}

SDL_FPoint Camera::worldToScreen(const SDL_FPoint& worldPos) const noexcept{
	SDL_FPoint screenPos = {};
	screenPos.x = (worldPos.x - viewX_) * zoom_;
	screenPos.y = (worldPos.y - viewY_) * zoom_;
	return screenPos;
}

bool Camera::isVisible(const SDL_FRect& worldRect) const noexcept{
	if(worldRect.x + worldRect.w < viewX_ || worldRect.x > viewX_ + viewWidth_ ||
	   worldRect.y + worldRect.h < viewY_ || worldRect.y > viewY_ + viewHeight_) {
		return false;
	}
	return true;
}



