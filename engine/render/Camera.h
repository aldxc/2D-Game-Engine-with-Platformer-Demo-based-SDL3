#pragma once
#include <SDL3/SDL.h>
#include "core/Rect.h"
#include "core/Vec2.h"

// 目前本项目camera属于场景系统的一部分，负责管理视口位置、缩放等信息，提供坐标转换、摄像机跟随、边界限制等功能
class Camera {
public:
	explicit Camera() noexcept = default ;
	~Camera() noexcept = default;

	// 禁用拷贝与移动
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&) = delete;
	Camera& operator=(Camera&&) = delete;

	// 初始化视口位置、大小和缩放级别
	bool init(float x, float y, float width, float height, float zoom = 1.0f) noexcept; 

	// 设置视口位置和缩放
	void setViewport(float x, float y, float width, float height, float zoom = 1.0f) noexcept; 
	void setPosition(float x, float y) noexcept; 
	void setWorldBounds(float x, float y, float width, float height) noexcept; 

	// 跟随目标，smooth表示是否平滑跟随，followSpeed表示跟随速度
	void followTarget(const SDL_FRect& target, float dt, bool smooth = false, float followSpeed = 5.0f) noexcept; 
	// 将视口位置限制在世界边界内
	void clampToBounds() noexcept; 

	// 坐标转换
	Rect worldToScreen(const SDL_FRect& worldRect) const noexcept;
	SDL_FPoint worldToScreen(const SDL_FPoint& worldPos) const noexcept;

	// 获取视口信息
	Rect getViewport() const noexcept { return Rect{ m_viewX, m_viewY, m_viewWidth, m_viewHeight }; }
	// 判断世界坐标矩形、坐标点是否在视口内
	bool isVisible(const Rect& worldRect) const noexcept; 
	bool isVisible(const Vec2& worldPos) const noexcept; 

	// 后续加入
	// 立即把相机居中到目标，不做平滑
	// 手动移动相机
	// 震动功能，提供震动强度、持续时间等参数，后续增加更多的震动效果，如基于距离衰减的震动、基于对象状态触发的震动等
private:
	// 视口位置、大小和缩放，单位为世界坐标
	float m_viewX = 0.0f; 
	float m_viewY = 0.0f; 
	float m_zoom = 1.0f; 
	float m_viewWidth = 0.0f;
	float m_viewHeight = 0.0f; 

	// 世界边界
	SDL_FRect m_worldBounds = { 0.0f, 0.0f, 0.0f, 0.0f }; 
	bool m_useWorldBounds = true; 

	// 是否跟随目标
	bool m_followTarget = false; 
	// 是否平滑跟随
	bool m_smoothFollow = false; 
	// 跟随速度，数值越大跟随越快
	float m_followSpeed = 5.0f; 
	// 死区，目标在死区内时摄像机不移动
	SDL_FRect m_deadZone = { 0.0f, 0.0f, 0.0f, 0.0f }; 

	//震动相关，后续加入。。。
};