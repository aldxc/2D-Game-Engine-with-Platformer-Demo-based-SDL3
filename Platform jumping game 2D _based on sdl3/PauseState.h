#pragma once
#include <array>
#include "core/State.h"
#include "StateType.h"
#include "Config.h"

class PauseState : public State<StateType> {
public:
	explicit PauseState() noexcept;
	~PauseState() noexcept override = default;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
};