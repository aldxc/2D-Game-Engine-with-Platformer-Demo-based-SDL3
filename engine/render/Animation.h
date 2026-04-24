#pragma once
#include <vector>
#include <unordered_map>
#include <SDL3/SDL.h>
#include "core/Rect.h"

enum class PlayerAnimationState { IDLE = 0, RUN = 1, JUMP = 2, FALL = 3, ATTACK = 4, CLIMB = 5, SPRINT = 6, FLASH = 7, HIT = 8 , WIN = 9};

class Animation {
public:
	struct AnimationClip {
		std::vector<Rect> frames; 
		// 每帧持续时间，单位为秒
		float frameDuration; 
		bool loop = true;
		AnimationClip() : frames({}), frameDuration(0.2f), loop(true) {};
		AnimationClip(std::vector<Rect> rects) : frames(rects), frameDuration(0.2f), loop(true) {};
	};

	explicit Animation() = default;
	~Animation() = default;

	// 禁用拷贝与移动
	Animation(const Animation&) = delete;
	Animation& operator=(const Animation&) = delete;
	Animation(Animation&&) = delete;
	Animation& operator=(Animation&&) = delete;

	bool init() noexcept;

	// 播放指定的动画剪辑，重置动画状态
	void play(const AnimationClip& clip) noexcept; 

	void update(double dt) noexcept;

	const Rect& getCurrentFrameRect() const noexcept;

	int getCurrentFrameIndex() const noexcept { return m_currentFrameIndex; }
	bool isFinished() const noexcept;
private:
	// 当前播放的动画剪辑
	AnimationClip m_currentClip; 
	// 当前帧索引
	size_t m_currentFrameIndex = 0; 
	// 当前帧已播放的时间
	double m_elapsed = 0.0f;
	// 动画是否已完成，非循环动画在播放完最后一帧后会设置为true，循环动画始终为false
	bool m_finished = false; 
};