#include <algorithm>
#include "Camera.h"

bool Camera::init(float x, float y, float width, float height, float zoom) noexcept{
	setViewport(x, y, width, height, zoom);
	return true;
}

void Camera::setViewport(float x, float y, float width, float height, float zoom) noexcept {
	m_viewX = x;
	m_viewY = y;
	m_viewWidth = width;
	m_viewHeight = height;
	m_zoom = zoom;
	// 死区设置为视口中心的50%区域，后续增加自定义死区设置功能
	m_deadZone = { m_viewWidth * 0.25f, m_viewHeight * 0.25f, m_viewWidth * 0.5f, m_viewHeight * 0.5f }; 
}

void Camera::setPosition(float x, float y) noexcept {
	m_viewX = x;
	m_viewY = y;
}

void Camera::setWorldBounds(float x, float y, float width, float height) noexcept{
	m_worldBounds = { x, y, width, height };
	m_useWorldBounds = true;
}

void Camera::followTarget(const SDL_FRect& target, float dt, bool smooth, float followSpeed) noexcept{
	if(target.x + target.w > m_viewX + m_deadZone.x + m_deadZone.w) {
		float targetX = target.x + target.w - (m_viewX + m_deadZone.x + m_deadZone.w);
		m_viewX += smooth ? (targetX * followSpeed * dt) : targetX;
	} else if(target.x < m_viewX + m_deadZone.x) {
		float targetX = target.x - (m_viewX + m_deadZone.x);
		m_viewX += smooth ? (targetX * followSpeed * dt) : targetX;
	}
	// 在跟随目标后限制摄像机位置在世界边界内
	clampToBounds();
}

void Camera::clampToBounds() noexcept{
	if(m_useWorldBounds) {
		m_viewX = std::clamp(m_viewX, m_worldBounds.x, m_worldBounds.x + m_worldBounds.w - m_viewWidth);
		m_viewY = std::clamp(m_viewY, m_worldBounds.y, m_worldBounds.y + m_worldBounds.h - m_viewHeight);
	}
}

Rect Camera::worldToScreen(const SDL_FRect& worldRect) const noexcept{
	Rect screenRect = {};
	screenRect.setX((worldRect.x - m_viewX) * m_zoom);
	screenRect.setY((worldRect.y - m_viewY) * m_zoom);
	screenRect.setW(worldRect.w * m_zoom);
	screenRect.setH(worldRect.h * m_zoom);

	return screenRect;
}

SDL_FPoint Camera::worldToScreen(const SDL_FPoint& worldPos) const noexcept{
	SDL_FPoint screenPos = {};
	screenPos.x = (worldPos.x - m_viewX) * m_zoom;
	screenPos.y = (worldPos.y - m_viewY) * m_zoom;
	return screenPos;
}

bool Camera::isVisible(const Rect& worldRect) const noexcept{
	if(worldRect.x() + worldRect.w() < m_viewX || worldRect.x() > m_viewX + m_viewWidth ||
	   worldRect.y() + worldRect.h() < m_viewY || worldRect.y() > m_viewY + m_viewHeight) {
		return false;
	}
	return true;
}

bool Camera::isVisible(const Vec2& worldPos) const noexcept{
	if(worldPos.x() < m_viewX || worldPos.x() > m_viewX + m_viewWidth ||
	   worldPos.y() < m_viewY || worldPos.y() > m_viewY + m_viewHeight) {
		return false;
	}
	return true;
}


