#pragma once
#include <memory>
#include "StateMachine.h"
#include "UIManager.h"

class Game {
public:
	Game();
	~Game();
	void Run() noexcept;
	void renderer() const noexcept;
	void handleInput() noexcept;
	void update() noexcept;
private:
	bool init() noexcept;
	bool isRunning_ = true;
	std::unique_ptr<StateMachine> stateMachine_;//后续增加事件管理器，不需要通过状态机来处理输入事件
	std::unique_ptr<UIManager> uiMananger_; //ui管理器
	SubscriptionId quitSubscriptionId_; //退出事件订阅ID
};