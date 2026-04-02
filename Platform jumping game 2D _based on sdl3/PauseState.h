#pragma once
#include <array>
#include "State.h"
#include "Config.h"

class PauseState : public State {
public:
	PauseState() noexcept;
	~PauseState() noexcept override = default;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
};