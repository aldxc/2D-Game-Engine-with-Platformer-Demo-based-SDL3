#pragma once
#include <vector>
#include <unordered_map>
#include <SDL3/SDL.h>

enum class PlayerAnimationState { IDLE, RUN, JUMP, FALL };

class Animation {
public:
	struct AnimationClip {
		std::vector<SDL_FRect> frames; 
		float frameDuration; // 每帧持续时间，单位为秒
		bool loop = true;
		AnimationClip() : frames({}), frameDuration(0.2f), loop(true) {};
		AnimationClip(std::vector<SDL_FRect> rects) : frames(rects), frameDuration(0.2f), loop(true) {};
	};

	explicit Animation() = default;
	~Animation() = default;
	// 禁用拷贝与移动
	Animation(const Animation&) = delete;
	Animation& operator=(const Animation&) = delete;
	Animation(Animation&&) = delete;
	Animation& operator=(Animation&&) = delete;
	bool init() noexcept;

	void play(const AnimationClip& clip) noexcept; // 播放指定的动画剪辑，重置动画状态
	void update(float dt) noexcept;
	const SDL_FRect& getCurrentFrameRect() const noexcept;
	bool isFinished() const noexcept;
private:
	AnimationClip currentClip_; // 当前播放的动画剪辑
	std::size_t currentFrameIndex_ = 0; // 当前帧索引
	float elapsed_ = 0.0f; // 当前帧已播放的时间
	bool finished_ = false; // 动画是否已完成，非循环动画在播放完最后一帧后会设置为true，循环动画始终为false
	//std::unordered_map<PlayerAnimationState, AnimationClip> animationClips_; // 不同玩家动画状态对应的动画剪辑映射
};