#include <string>
#include "Game.h"
#include "Config.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "render/Renderer.h"
#include "physics/Physics.h"

Game::Game() : renderContext_{ renderer_, camera_ }, gameSession_(resourceManager_), audioService_(audioManager_, resourceManager_, eventManager_){
	if (!init()) {
		SDL_Log("Failed to initialize game.");
		return;
	}
}

Game::~Game() {
	eventManager_.unsubscribe(quitSubscriptionId_);
}

void Game::Run() noexcept {
	while (isRunning_) {
		auto now = std::chrono::high_resolution_clock::now();
		double frameDt = std::chrono::duration<double>(now - lastFrameTime_).count();
		lastFrameTime_ = now;

		if (frameDt > 0.25) {
			frameDt = 0.25;
		}

		accumulator_ += frameDt;

		handleInput();
		update();
		uiMananger_->update(frameDt);

		updateCurrentFPS(frameDt);

		renderer();

		LogicUpdateFrequencyControl(now);
	}
}

void Game::handleInput() noexcept {

	if (inputManager_.getESCPressed()) {
		inputManager_.consumeESCKeyPress(); // 消费ESC键按下事件，避免持续按键导致的重复输入问题
		if (stateMachine_->getTopStateType() == StateType::PLAYING) {
			eventManager_.triggerEvent(Event{ EventType::Audio_PlaySfx, SfxId::UIButtonClick });
			eventManager_.triggerEvent(Event{ EventType::Audio_PauseBgm });
			eventManager_.triggerEvent({ EventType::State_Transition, StateRequest{ StateOperator::Push, StateType::PAUSE } });
			eventManager_.triggerEvent({ EventType::UI_Show, UIType::PAUSE });
		}else if(stateMachine_->getTopStateType() == StateType::PAUSE) {
			eventManager_.triggerEvent(Event{ EventType::Audio_PlaySfx, SfxId::UIButtonClick });
			eventManager_.triggerEvent(Event{ EventType::Audio_ResumeBgm });
			eventManager_.triggerEvent({ EventType::State_Transition, StateRequest{ StateOperator::Pop } });
			eventManager_.triggerEvent({ EventType::UI_Show, UIType::PLAYING });
		}
	}

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			isRunning_ = false;
			continue;
		}

		inputManager_.processInput(event, renderer_);
	}

	uiMananger_->handleInput();
	eventManager_.update();
}

void Game::update() noexcept {
	// 使用固定时间步长更新游戏逻辑，确保游戏在不同帧率下的行为一致
	while (accumulator_ >= Config::DELTAFREAM) {
		stateMachine_->update(Config::DELTAFREAM);
		inputManager_.resetInputState();//在每次逻辑更新后重置输入状态，确保输入状态只在当前逻辑帧内有效，避免输入状态在多帧之间持续导致的重复输入问题
		GlobalTime_ += Config::DELTAFREAM;
		accumulator_ -= Config::DELTAFREAM;
	}
}

void Game::renderer() const noexcept {
	renderer_.beginRender();

	stateMachine_->render();
	uiMananger_->render();
	renderer_.renderText("FPS: " + std::to_string(std::min(currentFPS_, MAX_FPS_)) + "/" + std::to_string(MAX_FPS_), SDL_FRect{10, 10, 100, 30}, SDL_Color({255, 255, 255, 255}), 20);

	renderer_.restoreDefaultAndPresent();
}

bool Game::init() noexcept {
	if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
		return false;
	}

	if (!renderer_.init(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT, Config::DEFAULT_TEXT_SIZE)) {
		SDL_Log("Failed to initialize renderer.");
		return false;
	}
	if(!audioManager_.init(Config::MAX_AUDIO_SFX_NUMS)) {
		SDL_Log("Failed to initialize audio manager.");
		return false;
	}
	if (!eventManager_.init()) {
		SDL_Log("Failed to initialize event manager.");
		return false;
	}
	if (!inputManager_.init()) {
		SDL_Log("Failed to initialize input manager.");
		return false;
	}
	if(!resourceManager_.init()) {
		SDL_Log("Failed to initialize resource manager.");
		return false;
	}
	if(!physicsEngine_.init(Config::GRAVITY)) {
		SDL_Log("Failed to initialize physics engine.");
		return false;
	}
	if (!camera_.init(0, 0, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT)) {
		SDL_Log("Failed to initialize camera.");
		return false;
	}
	if(!gameSession_.init()) {
		SDL_Log("Failed to initialize game session.");
		return false;
	}

	quitSubscriptionId_ = eventManager_.subscribe(EventType::App_Quit, [this](const Event&) {
		isRunning_ = false;
	});

	stateMachine_ = std::make_unique<GameStateMachine>(StateType::MENU, renderContext_, physicsEngine_, inputManager_, eventManager_, resourceManager_, gameSession_);
	uiMananger_ = std::make_unique<GameUIManager>(UIType::MENU, inputManager_, eventManager_, renderer_, gameSession_);
	lastFrameTime_ = std::chrono::high_resolution_clock::now();
	auto mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
	MAX_FPS_ = !mode ? 60 : std::min(Config::TARGET_RENDER_FPS, static_cast<uint32_t>(mode->refresh_rate)); // 如果无法获取显示模式信息，默认使用较低的帧率限制

	currentFPS_ = 0;
	fpsAccumulatedTime_ = 0;
	fpsAccumulatedTime_ = 0;

	return true;
}

void Game::LogicUpdateFrequencyControl(const std::chrono::time_point<std::chrono::high_resolution_clock>& frameStartTime){
	const auto targetFrameTime = std::chrono::duration<double>(1.0 / Config::TARGET_RENDER_FPS);
	const auto frameElapsed = std::chrono::high_resolution_clock::now() - frameStartTime;

	if (frameElapsed < targetFrameTime) {
		const auto remainingTime = std::chrono::duration_cast<std::chrono::milliseconds>(targetFrameTime - frameElapsed);
		if (remainingTime.count() > 0) {
			SDL_Delay(static_cast<Uint32>(remainingTime.count()));
		}
	}
}

void Game::updateCurrentFPS(double frameDt) noexcept {
	// 平滑统计FPS，避免过于频繁的波动，使用指数移动平均平滑FPS值
	++fpsFrameCount_;
	fpsAccumulatedTime_ += frameDt;

	if (fpsAccumulatedTime_ >= 0.25) {
		currentFPS_ = static_cast<int>(fpsFrameCount_ / fpsAccumulatedTime_ + 0.5);
		fpsFrameCount_ = 0;
		fpsAccumulatedTime_ = 0.0;
	}
}

