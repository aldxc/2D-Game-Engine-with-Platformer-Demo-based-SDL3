#pragma once
#include <SDL3/SDL.h>
#include <core/Rect.h>

// 后续可属于组件，挂载在玩家、敌人等对象上，提供视口信息、坐标转换等功能，后续增加摄像机跟随、边界限制等功能
// 本项目camera属于场景系统的一部分，负责管理视口位置、缩放等信息，提供坐标转换等功能，后续增加摄像机跟随、边界限制等功能
class Camera {
public:
	explicit Camera() noexcept = default ;
	~Camera() noexcept = default;

	bool init(float x, float y, float width, float height, float zoom = 1.0f) noexcept; // 供系统初始化时调用，设置初始视口位置、缩放级别等信息，后续增加根据窗口大小自动调整视口等功能

	// 设置视口位置和缩放
	void setViewport(float x, float y, float width, float height, float zoom = 1.0f) noexcept; // 初始化
	void setPosition(float x, float y) noexcept; // 设置视口位置
	void setWorldBounds(float x, float y, float width, float height) noexcept; // 设置世界边界，摄像机位置将被限制在这个边界内，后续增加启用/禁用世界边界限制的功能

	void followTarget(const SDL_FRect& target, float dt, bool smooth = false, float followSpeed = 5.0f) noexcept; // 跟随目标，smooth表示是否平滑跟随，followSpeed表示跟随速度
	void clampToBounds() noexcept; // 将视口位置限制在世界边界内

	// 坐标转换
	Rect worldToScreen(const SDL_FRect& worldRect) const noexcept;
	SDL_FPoint worldToScreen(const SDL_FPoint& worldPos) const noexcept;
	//SDL_FPoint screenToWorld(const SDL_FPoint& screenPos) const noexcept; 

	// 获取视口信息
	Rect getViewport() const noexcept { return Rect{ viewX_, viewY_, viewWidth_, viewHeight_ }; }
	bool isVisible(const SDL_FRect& worldRect) const noexcept; // 判断世界坐标矩形是否在视口内，后续增加更多的可见性判断功能，如基于对象中心点、基于对象边界等不同的可见性判断方式

	// 后续加入
	// 立即把相机居中到目标，不做平滑
	// 手动移动相机
	// 震动功能，提供震动强度、持续时间等参数，后续增加更多的震动效果，如基于距离衰减的震动、基于对象状态触发的震动等

private:
	float viewX_ = 0.0f; // 视口左上角X坐标 -- 世界坐标
	float viewY_ = 0.0f; // 视口左上角Y坐标 -- 世界坐标
	float zoom_ = 1.0f; // 视口缩放级别，默认为1.0表示不缩放
	float viewWidth_ = 0.0f; // 视口宽度，一般为逻辑宽度
	float viewHeight_ = 0.0f; // 视口高度，一般为逻辑高度

	SDL_FRect worldBounds_ = { 0.0f, 0.0f, 0.0f, 0.0f }; // 世界边界
	bool useWorldBounds_ = true; // 是否启用世界边界限制

	bool followTarget_ = false; // 是否跟随目标
	bool smoothFollow_ = false; // 是否平滑跟随
	float followSpeed_ = 5.0f; // 跟随速度，数值越大跟随越快
	SDL_FRect deadZone_ = { 0.0f, 0.0f, 0.0f, 0.0f }; // 死区，目标在死区内时摄像机不移动

	//震动相关，后续加入。。。
};