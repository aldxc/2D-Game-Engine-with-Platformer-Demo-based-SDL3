#pragma once
#include <array>
#include "core/State.h"
#include "StateType.h"

class Renderer;

class MenuState : public State<StateType> {
public:
	explicit MenuState(Renderer& r) noexcept;
	~MenuState() noexcept override = default;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
private:
	Renderer& renderer_;
};