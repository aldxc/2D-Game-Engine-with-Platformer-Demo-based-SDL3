#pragma once
#include <memory>
#include "StateMachine.h"

class Game {
public:
	Game();
	~Game() = default;
	void Run() noexcept;
private:
	bool init() noexcept;
	bool isRunning_ = true;
	std::unique_ptr<StateMachine> stateMachine_;//后续增加事件管理器，不需要通过状态机来处理输入事件
};