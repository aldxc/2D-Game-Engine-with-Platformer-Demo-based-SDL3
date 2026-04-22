#include "SelectState.h"
#include "render/Renderer.h"
#include "resource/Resource.h"
#include "input/Input.h"
#include "core/Rect.h"
#include "Config.h"

SelectState::SelectState(Renderer& renderer, Resource& rM, Input& iM) noexcept : State<StateType>(StateType::SELECT_HERO), renderer_(renderer), resourceManager_(rM), inputManager_(iM){
	playerTexture_ = resourceManager_.loadTexture(Config::PLAYER_TEXTURE_PATH, renderer_.getSDLRenderer());

	Animation::AnimationClip animationClip;
	const auto src = Config::PLAYER_IDLE_SRC;
	const std::array<float, Config::SELECT_BUTTONS_NUM> offsets = {
		Config::PLAYER_1_Y_OFFSET,
		Config::PLAYER_2_Y_OFFSET,
		Config::PLAYER_3_Y_OFFSET
	};
	for(int i = 0; i < Config::SELECT_BUTTONS_NUM; ++i) {
		animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1] + offsets[i]), static_cast<float>(src[2]), static_cast<float>(src[3])});
		animations_[i].play(animationClip);
		animationClip.frames.clear(); 
		animationStates_[i] = PlayerAnimationState::IDLE; 
	}
}

void SelectState::render() const noexcept{
	renderer_.clearStaticTexture();

	renderer_.clearDynamicTexture(); 
	for(int i = 0; i < Config::SELECT_BUTTONS_NUM; ++i) {
		Rect currentFrameRect = animations_[i].getCurrentFrameRect();
		renderer_.renderTexture(playerTexture_.get(), currentFrameRect, Config::SELECT_SHOW_RECT[i]);
	}
	// 皮肤1
	renderer_.renderRect(Config::SELECT_BUTTONS_RECT[0], renderer_.setColorAlpha(200, 100, 100, 255));
	// 皮肤2
	renderer_.renderRect(Config::SELECT_BUTTONS_RECT[1], renderer_.setColorAlpha(200, 100, 100, 255));
	// 皮肤3
	renderer_.renderRect(Config::SELECT_BUTTONS_RECT[2], renderer_.setColorAlpha(200, 100, 100, 255));
}

void SelectState::update(double dt) noexcept{
	// 根据输入状态更新动画状态
	updateAnimationState(dt);
}

void SelectState::updateAnimationState(float dt) noexcept {
	const std::array<float, Config::SELECT_BUTTONS_NUM> offsets = {
		Config::PLAYER_1_Y_OFFSET,
		Config::PLAYER_2_Y_OFFSET,
		Config::PLAYER_3_Y_OFFSET
	};
	for(int i = 0; i < Config::SELECT_BUTTONS_NUM; ++i) {
		if(animationStateChange(animationStates_[i], i)) {
			switch (animationStates_[i]) {
				case PlayerAnimationState::IDLE: {
					Animation::AnimationClip animationClip;
					const auto src = Config::PLAYER_IDLE_SRC;
					animationClip.frames.push_back(Rect{ static_cast<float>(src[0]), static_cast<float>(src[1] + offsets[i]), static_cast<float>(src[2]), static_cast<float>(src[3])});
					animations_[i].play(animationClip);
					break;	
				}
				case PlayerAnimationState::RUN: {
					Animation::AnimationClip animationClip;
					const auto src = Config::PLAYER_RUN_SRC;
					for(const auto& rect : src) {
						animationClip.frames.push_back(Rect{ static_cast<float>(rect[0]), static_cast<float>(rect[1] + offsets[i]), static_cast<float>(rect[2]), static_cast<float>(rect[3])});
					}
					animations_[i].play(animationClip);
					break;
				}
				default: {
					// 其他状态暂时不处理，后续可以根据需要添加更多的动画状态和对应的动画剪辑
					break;
				}
			}
		}
		animations_[i].update(dt);
	}
}

bool SelectState::animationStateChange(PlayerAnimationState& animationState_, int index) noexcept {
	auto [mouseX, mouseY] = inputManager_.getMousePosition();
	Rect bottonRect = Config::SELECT_BUTTONS_RECT[index];
	if (bottonRect.hasIntersection({ mouseX, mouseY })) {
		if (animationState_ != PlayerAnimationState::RUN) {
			animationState_ = PlayerAnimationState::RUN;
			return true;
		}
		return false;
	} else {
		if (animationState_ != PlayerAnimationState::IDLE) {
			animationState_ = PlayerAnimationState::IDLE;
			return true;
		}
		return false;
	}
}