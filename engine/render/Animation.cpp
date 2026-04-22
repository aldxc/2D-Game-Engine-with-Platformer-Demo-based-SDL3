#include "Animation.h"

bool Animation::init() noexcept{
	return true;
}

void Animation::update(double dt) noexcept{
	if (currentClip_.frames.empty()) {
		finished_ = true;
		// 如果当前动画剪辑没有帧，直接标记为完成并返回
		return; 
	}
	elapsed_ += dt;
	while (elapsed_ >= currentClip_.frameDuration) {
		elapsed_ -= currentClip_.frameDuration;
		currentFrameIndex_++;
		if (currentFrameIndex_ >= currentClip_.frames.size()) {
			if (currentClip_.loop) {
				currentFrameIndex_ = 0; // 循环播放，重置帧索引
			} else {
				currentFrameIndex_ = currentClip_.frames.size() - 1; // 非循环播放，保持在最后一帧
				finished_ = true; // 标记动画完成
				break;
			}
		}
	}
}

void Animation::play(const AnimationClip& clip) noexcept{
	currentClip_ = clip;
	currentFrameIndex_ = 0;
	elapsed_ = 0.0f;
	finished_ = clip.frames.empty();
}

const Rect& Animation::getCurrentFrameRect() const noexcept{
	if (currentClip_.frames.empty()) {
		static const Rect emptyRect{ 0, 0, 0, 0 };
		// 如果当前动画剪辑没有帧，返回一个空的矩形
		return emptyRect; 
	}
	else {
		// 返回当前帧的矩形
		return currentClip_.frames[currentFrameIndex_]; 
	}
}

bool Animation::isFinished() const noexcept{
	return finished_;
}

