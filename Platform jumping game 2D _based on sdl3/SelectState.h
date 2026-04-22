#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include <array>
#include "core/State.h"
#include "render/Animation.h"
#include "StateType.h"

class Renderer;
class Resource;
class Input;


class SelectState : public State<StateType> {
public:
	explicit SelectState(Renderer& renderer, Resource& rM, Input& iM) noexcept;
	~SelectState() noexcept override = default;
	void render() const noexcept override final;
	void update(double dt) noexcept override final;
	void updateAnimationState(float dt) noexcept; 
	bool animationStateChange(PlayerAnimationState& animationState_, int index) noexcept; 
private:
	Renderer& renderer_;
	Resource& resourceManager_;
	Input& inputManager_;

	std::shared_ptr<SDL_Texture> playerTexture_ = nullptr;

	std::array<Animation, 3> animations_;
	std::array<PlayerAnimationState, 3> animationStates_;
};