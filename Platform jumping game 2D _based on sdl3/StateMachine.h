#pragma once
#include <memory>
#include "State.h"

//负责游戏状态的切换，状态的更新和渲染
class StateMachine {
public:
	StateMachine();
	~StateMachine() = default;
	void handleInput();
	void update();
	void render();
private:
	std::unique_ptr<State> currentState_;
};