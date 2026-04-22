#pragma once
#include <memory>
#include <chrono>
#include <cstdint>
#include "core/StateMachine.h"
#include "core/UIManager.h"
#include "core/State.h"
#include "core/Context.h"
#include "core/EventManager.h"
#include "render/Renderer.h"
#include "resource/Resource.h"
#include "input/Input.h"
#include "physics/Physics.h"
#include "render/Animation.h"
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
	//系统资源去除单例，维护在此处，后续增加资源管理器等功能
	RenderContext renderContext_; //
	Renderer renderer_; // 渲染器实例
	Audio audioManager_; // 音频管理器实例

	EventManager eventManager_; // 事件管理器实例

	Input inputManager_; // 输入管理器实例
	Resource resourceManager_; // 资源管理器实例
	Physics physicsEngine_; // 物理引擎实例
	Camera camera_; // 摄像机实例
	GameSession gameSession_; // 游戏会话实例，负责管理游戏的整体状态和流程
	AudioService audioService_; // 音频服务实例，负责处理游戏中的音频播放逻辑

	using GameStateMachine = StateMachine<StateType, GameStateFactory>;
	using GameUIManager = UIManager<UIType, GameUIFactory>;
private:
	bool init() noexcept;
	bool isRunning_ = true;
	void LogicUpdateFrequencyControl(const std::chrono::time_point<std::chrono::high_resolution_clock>& frameStartTime); // 控制逻辑更新频率，使用固定时间步长更新
	void updateCurrentFPS(double frameDt) noexcept; // 更新当前FPS统计，使用指数移动平均平滑FPS值，避免过于频繁的波动
private:
	std::unique_ptr<GameStateMachine> stateMachine_;//后续增加事件管理器，不需要通过状态机来处理输入事件
	std::unique_ptr<GameUIManager> uiMananger_; //ui管理器
	SubscriptionId quitSubscriptionId_; //退出事件订阅ID
	double GlobalTime_ = 0.0f; //全局时间，单位秒
	double accumulator_ = 0.0f; //时间累积器，用于固定时间步长更新
	std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime_; //上次帧时间点
	uint32_t MAX_FPS_ = 120; //最大帧率，控制游戏循环的频率，防止过高的帧率导致CPU占用过高

	//FPS统计相关 平滑统计
	uint32_t currentFPS_ = 0;
	uint32_t fpsFrameCount_ = 0;
	double fpsAccumulatedTime_ = 0.0;
};