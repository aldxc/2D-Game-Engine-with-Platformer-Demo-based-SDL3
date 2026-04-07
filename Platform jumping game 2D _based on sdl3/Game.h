#pragma once
#include <memory>
#include <chrono>
#include "core/StateMachine.h"
#include "core/UIManager.h"
#include "core/State.h"
#include "render/Animation.h"
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
	void LogicUpdateFrequencyControl(const std::chrono::time_point<std::chrono::high_resolution_clock>& frameStartTime); // 控制逻辑更新频率，使用固定时间步长更新
	void updateCurrentFPS(double frameDt) noexcept; // 更新当前FPS统计，使用指数移动平均平滑FPS值，避免过于频繁的波动
	std::unique_ptr<GameStateMachine> stateMachine_;//后续增加事件管理器，不需要通过状态机来处理输入事件
	std::unique_ptr<GameUIManager> uiMananger_; //ui管理器
	SubscriptionId quitSubscriptionId_; //退出事件订阅ID
	float GlobalTime_ = 0.0f; //全局时间，单位秒
	float accumulator_ = 0.0f; //时间累积器，用于固定时间步长更新
	std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime_; //上次帧时间点
	int MAX_FPS_ = 120; //最大帧率，控制游戏循环的频率，防止过高的帧率导致CPU占用过高

	//FPS统计相关 平滑统计
	int currentFPS_ = 0;
	int fpsFrameCount_ = 0;
	double fpsAccumulatedTime_ = 0.0;

	//系统资源去除单例，维护在此处，后续增加资源管理器等功能

};