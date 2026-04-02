#pragma once
#include "State.h"

class PlayingState : public State {
public:
	PlayingState() noexcept;
	~PlayingState() noexcept override = default;
	void render() const noexcept override final;
	void update(float dt) noexcept override final;
};
