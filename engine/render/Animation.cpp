#include "Animation.h"

bool Animation::init() noexcept{
	return true;
}

void Animation::update(double dt) noexcept{
	if (m_currentClip.frames.empty()) {
		m_finished = true;
		// 如果当前动画剪辑没有帧，直接标记为完成并返回
		return; 
	}
	m_elapsed += dt;
	while (m_elapsed >= m_currentClip.frameDuration) {
		m_elapsed -= m_currentClip.frameDuration;
		m_currentFrameIndex++;
		if (m_currentFrameIndex >= m_currentClip.frames.size()) {
			if (m_currentClip.loop) {
				m_currentFrameIndex = 0; // 循环播放，重置帧索引
			} else {
				m_currentFrameIndex = m_currentClip.frames.size() - 1; // 非循环播放，保持在最后一帧
				m_finished = true; // 标记动画完成
				break;
			}
		}
	}
}

void Animation::play(const AnimationClip& clip) noexcept{
	m_currentClip = clip;
	m_currentFrameIndex = 0;
	m_elapsed = 0.0f;
	m_finished = clip.frames.empty();
}

const Rect& Animation::getCurrentFrameRect() const noexcept{
	if (m_currentClip.frames.empty()) {
		static const Rect emptyRect{ 0, 0, 0, 0 };
		// 如果当前动画剪辑没有帧，返回一个空的矩形
		return emptyRect; 
	}
	else {
		// 返回当前帧的矩形
		return m_currentClip.frames[m_currentFrameIndex]; 
	}
}

bool Animation::isFinished() const noexcept{
	return m_finished;
}

