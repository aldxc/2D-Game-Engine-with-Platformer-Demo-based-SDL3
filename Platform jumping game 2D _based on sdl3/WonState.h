#pragma once
#include "core/State.h"
#include "StateType.h"

class Renderer;

class WonState : public State<StateType> {
public:
	explicit WonState(Renderer& renderer) noexcept;
	~WonState() noexcept override = default;
	void render() const noexcept override final;
	void update(double dt) noexcept override final;
private:
	Renderer& m_renderer;
};