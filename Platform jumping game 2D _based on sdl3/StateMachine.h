#pragma once
#include <memory>
#include "State.h"

//负责游戏状态的切换，状态的更新和渲染
class StateMachine {
public:
	StateMachine();
	~StateMachine() = default;
	void update();
	void render();
private:
	std::unique_ptr<State> createState(StateType type); //工厂函数，创造状态实例，智能指针自动管理销毁
	std::unique_ptr<State> currentState_;
};