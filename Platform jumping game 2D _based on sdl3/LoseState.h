#pragma once
#include "core/State.h"
#include "StateType.h"

class Renderer;

class LoseState : public State<StateType> {
public:
	explicit LoseState(Renderer& renderer) noexcept;
	~LoseState() noexcept override = default;
	void render() const noexcept override final;
	void update(double dt) noexcept override final;
private:
	Renderer& renderer_;
};