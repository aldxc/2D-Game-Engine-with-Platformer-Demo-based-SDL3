#pragma once
#include "core/State.h"
#include "StateType.h"

class LoseState : public State<StateType> {
public:
	explicit LoseState() noexcept;
	~LoseState() noexcept override = default;
	void render() const noexcept override final;
	void update(float dt) noexcept override final;
private:

};