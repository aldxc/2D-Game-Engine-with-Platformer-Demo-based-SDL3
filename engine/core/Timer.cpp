#include <utility>
#include "Timer.h"


Timer::Timer() noexcept {
	m_isActive = false;
	m_durationTime = 0.0f;
	m_elapsedTime = 0.0f;
	m_looped = false;
}

void Timer::start(double duration, bool isloop) noexcept {
	m_isActive = true;
	m_durationTime = duration;
	m_elapsedTime = 0.0f;
	m_looped = isloop;
}

void Timer::stop() noexcept {
	m_isActive = false;
	m_durationTime = 0.0f;
	m_elapsedTime = 0.0f;
}

void Timer::update(double dt) noexcept {
	if (!m_isActive) {
		return;
	}
	// 防止dt过大导致时间跳跃，尤其是在调试或帧率不稳定时
	const double maxDt = 0.02f; 
	dt = std::min(dt, maxDt);

	m_elapsedTime += dt;
	if (m_elapsedTime >= m_durationTime) {
		// 处理可能的时间溢出，保持剩余时间
		m_elapsedTime -= m_durationTime; 

		// 这里可以触发定时器事件，例如调用回调函数等，后续增加相关功能

		if (!m_looped) {
			stop(); // 定时器到期，停止并重置状态
		}
	}
}