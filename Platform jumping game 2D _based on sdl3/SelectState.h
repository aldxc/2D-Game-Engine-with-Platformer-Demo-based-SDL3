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
	Renderer& m_renderer;
	Resource& m_resourceManager;
	Input& m_inputManager;

	std::shared_ptr<SDL_Texture> m_playerTexture = nullptr;

	std::array<Animation, 3> m_animations;
	std::array<PlayerAnimationState, 3> m_animationStates;
};