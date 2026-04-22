#pragma once
#include <memory>
#include <chrono>
#include <cstdint>
#include "core/StateMachine.h"
#include "core/UIManager.h"
#include "core/Context.h"
#include "core/EventManager.h"
#include "render/Renderer.h"
#include "resource/Resource.h"
#include "input/Input.h"
#include "physics/Physics.h"
#include "render/Camera.h"
#include "audio/Audio.h"
#include "GameStateFactory.h"
#include "UIType.h"
#include "UIStateFactory.h"
#include "GameSession.h"
#include "AudioService.h"

class Game {
public:
	explicit Game();
	~Game();
	void Run() noexcept;
	void renderer() const noexcept;
	void handleInput() noexcept;
	void update() noexcept;
private:
	//引擎系统资源维护在此处，注意声明顺序
	RenderContext renderContext_; 
	Renderer renderer_; 
	Audio audioManager_; 
	EventManager eventManager_; 
	Input inputManager_; 
	Resource resourceManager_; 
	Physics physicsEngine_; 
	Camera camera_; 
	GameSession gameSession_; 
	AudioService audioService_; 

	using GameStateMachine = StateMachine<StateType, GameStateFactory>;
	using GameUIManager = UIManager<UIType, GameUIFactory>;
private:
	bool init() noexcept;
	
	// 控制逻辑更新频率，使用固定时间步长更新
	void LogicUpdateFrequencyControl(const std::chrono::time_point<std::chrono::high_resolution_clock>& frameStartTime); 
	// 更新当前FPS统计，使用指数移动平均平滑FPS值，避免过于频繁的波动
	void updateCurrentFPS(double frameDt) noexcept; 
private:
	// 运行标志
	bool isRunning_ = true;

	std::unique_ptr<GameStateMachine> stateMachine_;
	std::unique_ptr<GameUIManager> uiMananger_; 
	SubscriptionId quitSubscriptionId_; 

	//全局时间，单位秒
	double globalTime_ = 0.0f; 
	//时间累积器，用于固定时间步长更新
	double accumulator_ = 0.0f; 
	//上次帧时间点
	std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime_; 
	//最大帧率，控制游戏循环的频率，防止过高的帧率导致CPU占用过高
	uint32_t MAX_FPS_ = 120; 

	//FPS统计相关 平滑统计
	uint32_t currentFPS_ = 0;
	uint32_t fpsFrameCount_ = 0;
	double fpsAccumulatedTime_ = 0.0;
};