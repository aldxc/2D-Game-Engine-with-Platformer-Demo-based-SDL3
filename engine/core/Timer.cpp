#include <utility>
#include "Timer.h"


Timer::Timer() noexcept {
	isActive_ = false;
	duration_ = 0.0f;
	elapsedTime_ = 0.0f;
	looped_ = false;
}

void Timer::start(double duration, bool isloop) noexcept {
	isActive_ = true;
	duration_ = duration;
	elapsedTime_ = 0.0f;
	looped_ = isloop;
}

void Timer::stop() noexcept {
	isActive_ = false;
	duration_ = 0.0f;
	elapsedTime_ = 0.0f;
}

void Timer::update(double dt) noexcept {
	if (!isActive_) {
		return;
	}
	// 防止dt过大导致时间跳跃，尤其是在调试或帧率不稳定时
	const double maxDt = 0.02f; 
	dt = std::min(dt, maxDt);

	elapsedTime_ += dt;
	if (elapsedTime_ >= duration_) {
		// 处理可能的时间溢出，保持剩余时间
		elapsedTime_ -= duration_; 

		// 这里可以触发定时器事件，例如调用回调函数等，后续增加相关功能

		if (!looped_) {
			stop(); // 定时器到期，停止并重置状态
		}
	}
}