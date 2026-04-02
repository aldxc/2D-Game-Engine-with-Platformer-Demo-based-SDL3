#pragma once
#include <array>
#include "State.h"

class MenuState : public State {
public:
	MenuState() noexcept;
	~MenuState() noexcept override = default;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
private:

};