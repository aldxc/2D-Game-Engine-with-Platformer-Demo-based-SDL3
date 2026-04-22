#pragma once
#include <array>
#include "core/State.h"
#include "StateType.h"
#include "Config.h"

class Renderer;

class PauseState : public State<StateType> {
public:
	explicit PauseState(Renderer& renderer) noexcept;
	~PauseState() noexcept override = default;
	void update(double dt) noexcept override final;
	void render() const noexcept override final;
private:
	Renderer& renderer_;
};