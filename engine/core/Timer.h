#pragma once
#include <chrono>

class Timer { // 后续使用全局时间戳 // 后续可加入回调函数等功能，支持定时器事件的触发
public:
	explicit Timer() noexcept;
	~Timer() noexcept = default;
	void start(float duration, bool isloop = false) noexcept; // 启动定时器，设置持续时间，单位秒
	void stop() noexcept; // 停止定时器，重置状态
	void update(float dt) noexcept; // 更新定时器状态，dt为每帧的时间增量，单位秒
	bool isActive() const noexcept { return isActive_; } // 检查定时器是否正在运行
	// 后续增加:暂停、恢复、重置、回调函数等功能
private:
	bool isActive_ = false; // 定时器是否正在运行
	float duration_ = 0.0f; // 定时器持续时间，单位秒
	float elapsedTime_ = 0.0f; // 已经过去的时间，单位秒
	bool looped_ = false; // 定时器是否循环，默认为非循环
};