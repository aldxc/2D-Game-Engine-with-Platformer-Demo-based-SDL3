#include "Animation.h"

bool Animation::init() noexcept{
	return true;
}

void Animation::update(float dt) noexcept{
	if (currentClip_.frames.empty()) {
		finished_ = true;
		return; // 如果当前动画剪辑没有帧，直接标记为完成并返回
	}
	elapsed_ += dt;
	while (elapsed_ >= currentClip_.frameDuration) {
		//SDL_Log("Animation index: %d", currentFrameIndex_);
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
		return emptyRect; // 如果当前动画剪辑没有帧，返回一个空的矩形
	}
	else {
		return currentClip_.frames[currentFrameIndex_]; // 返回当前帧的矩形
	}
}

bool Animation::isFinished() const noexcept{
	return finished_;
}

