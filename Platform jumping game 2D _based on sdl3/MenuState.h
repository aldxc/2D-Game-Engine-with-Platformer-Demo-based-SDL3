#pragma once
#include <array>
#include "core/State.h"
#include "StateType.h"

class MenuState : public State<StateType> {
public:
	MenuState() noexcept;
	~MenuState() noexcept override = default;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
private:

};