#pragma once
//#include <chrono>

// 计时器组件
// 后续使用全局时间戳，可加入回调函数等功能，支持定时器事件的触发
class Timer { 
public:
	explicit Timer() noexcept;
	~Timer() noexcept = default;

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(Timer&&) = delete;

	// 启动定时器，设置持续时间，单位秒
	void start(double duration, bool isloop = false) noexcept; 

	// 停止定时器，重置状态
	void stop() noexcept; 

	// 更新定时器状态
	void update(double dt) noexcept;

	// 检查定时器是否正在运行
	bool isActive() const noexcept { return isActive_; } 

	// 获取已经过去的时间，单位秒
	double getElapsedTime() const noexcept { return elapsedTime_; } 

	// 获取计时器时间
	double getDurationTime() const noexcept { return duration_; }

	// 后续增加:暂停、恢复、重置、回调函数等功能
private:
	// 定时器是否正在运行
	bool isActive_ = false; 

	// 定时器持续时间，单位秒
	double duration_ = 0.0f; 

	// 已经过去的时间，单位秒
	double elapsedTime_ = 0.0f; 

	// 定时器是否循环，默认为非循环
	bool looped_ = false; 
};