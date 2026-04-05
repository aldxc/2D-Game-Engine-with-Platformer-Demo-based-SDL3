#pragma once
#include <memory>
#include <chrono>
#include "core/StateMachine.h"
#include "core/UIManager.h"
#include "core/State.h"
#include "GameStateFactory.h"
#include "UIType.h"
#include "UIStateFactory.h"

class Game {
public:
	explicit Game();
	~Game();
	void Run() noexcept;
	void renderer() const noexcept;
	void handleInput() noexcept;
	void update() noexcept;
private:
	using GameStateMachine = StateMachine<StateType, GameStateFactory>;
	using GameUIManager = UIManager<UIType, GameUIFactory>;

	bool init() noexcept;
	bool isRunning_ = true;
	std::unique_ptr<GameStateMachine> stateMachine_;//后续增加事件管理器，不需要通过状态机来处理输入事件
	std::unique_ptr<GameUIManager> uiMananger_; //ui管理器
	SubscriptionId quitSubscriptionId_; //退出事件订阅ID
	float GlobalTime_ = 0.0f; //全局时间，单位秒
	float accumulator_ = 0.0f; //时间累积器，用于固定时间步长更新
	std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime_; //上次帧时间点
};