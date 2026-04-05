#pragma once
#include "core/State.h"
#include "StateType.h"

class WonState : public State<StateType> {
public:
	explicit WonState() noexcept;
	~WonState() noexcept override = default;
	void render() const noexcept override final;
	void update(float dt) noexcept override final;
private:

};