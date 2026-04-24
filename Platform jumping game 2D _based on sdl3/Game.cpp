#include <string>
#include "Game.h"
#include "Config.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "render/Renderer.h"
#include "physics/Physics.h"

Game::Game() : m_renderContext{ m_renderer, m_camera }, m_gameSession(m_resourceManager), m_audioService(m_audioManager, m_resourceManager, m_eventManager){
	if (!init()) {
		SDL_Log("Failed to initialize game.");
		return;
	}
}

Game::~Game() {
	m_eventManager.unsubscribe(m_quitSubscriptionId);
}

void Game::Run() noexcept {
	while (m_isRunning) {
		auto now = std::chrono::high_resolution_clock::now();
		double frameDt = std::chrono::duration<double>(now - m_lastFrameTime).count();
		m_lastFrameTime = now;

		if (frameDt > 0.25) {
			frameDt = 0.25;
		}

		m_accumulator += frameDt;

		handleInput();
		update();
		m_uiMananger->update(frameDt);

		updateCurrentFPS(frameDt);

		renderer();

		LogicUpdateFrequencyControl(now);
	}
}

void Game::handleInput() noexcept {

	if (m_inputManager.getESCPressed()) {
		// 消费ESC键按下事件，避免持续按键导致的重复输入问题
		m_inputManager.consumeESCKeyPress(); 
		if (m_stateMachine->getTopStateType() == StateType::PLAYING) {
			m_eventManager.triggerEvent(Event{ EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK });
			m_eventManager.triggerEvent(Event{ EventType::AUDIO_PAUSE_BGM });
			m_eventManager.triggerEvent({ EventType::STATE_TRANSITION, StateRequest{ StateOperator::PUSH, StateType::PAUSE } });
			m_eventManager.triggerEvent({ EventType::UI_SHOW, UIType::PAUSE });
		}else if(m_stateMachine->getTopStateType() == StateType::PAUSE) {
			m_eventManager.triggerEvent(Event{ EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK });
			m_eventManager.triggerEvent(Event{ EventType::AUDIO_RESUME_BGM });
			m_eventManager.triggerEvent({ EventType::STATE_TRANSITION, StateRequest{ StateOperator::POP } });
			m_eventManager.triggerEvent({ EventType::UI_SHOW, UIType::PLAYING });
		}
	}

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			m_isRunning = false;
			continue;
		}

		m_inputManager.processInput(event, m_renderer);
	}

	m_uiMananger->handleInput();
	m_eventManager.update();
}

void Game::update() noexcept {
	// 使用固定时间步长更新游戏逻辑，确保游戏在不同帧率下的行为一致
	while (m_accumulator >= Config::DELTAFREAM) {
		m_stateMachine->update(Config::DELTAFREAM);
		//在每次逻辑更新后重置输入状态，确保输入状态只在当前逻辑帧内有效，避免输入状态在多帧之间持续导致的重复输入问题
		m_inputManager.resetInputState();
		m_globalTime += Config::DELTAFREAM;
		m_accumulator -= Config::DELTAFREAM;
	}
}

void Game::renderer() const noexcept {
	m_renderer.beginRender();

	m_stateMachine->render();
	m_uiMananger->render();
	m_renderer.renderText("FPS: " + std::to_string(std::min(m_currentFPS, m_MAX_FPS)) + "/" + std::to_string(m_MAX_FPS), SDL_FRect{10, 10, 100, 30}, SDL_Color({255, 255, 255, 255}), 20);

	m_renderer.restoreDefaultAndPresent();
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

	if (!m_renderer.init(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT, Config::DEFAULT_TEXT_SIZE)) {
		SDL_Log("Failed to initialize renderer.");
		return false;
	}
	if(!m_audioManager.init(Config::MAX_AUDIO_SFX_NUMS)) {
		SDL_Log("Failed to initialize audio manager.");
		return false;
	}
	if (!m_eventManager.init()) {
		SDL_Log("Failed to initialize event manager.");
		return false;
	}
	if (!m_inputManager.init()) {
		SDL_Log("Failed to initialize input manager.");
		return false;
	}
	if(!m_resourceManager.init()) {
		SDL_Log("Failed to initialize resource manager.");
		return false;
	}
	if(!m_physicsEngine.init(Config::GRAVITY)) {
		SDL_Log("Failed to initialize physics engine.");
		return false;
	}
	if (!m_camera.init(0, 0, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT)) {
		SDL_Log("Failed to initialize camera.");
		return false;
	}
	if(!m_gameSession.init()) {
		SDL_Log("Failed to initialize game session.");
		return false;
	}

	m_quitSubscriptionId = m_eventManager.subscribe(EventType::APP_QUIT, [this](const Event&) {
		m_isRunning = false;
	});

	m_stateMachine = std::make_unique<GameStateMachine>(StateType::MENU, m_renderContext, m_physicsEngine, m_inputManager, m_eventManager, m_resourceManager, m_gameSession);
	m_uiMananger = std::make_unique<GameUIManager>(UIType::MENU, m_inputManager, m_eventManager, m_renderer, m_gameSession);
	m_lastFrameTime = std::chrono::high_resolution_clock::now();
	auto mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
	// 如果无法获取显示模式信息，默认使用较低的帧率限制
	m_MAX_FPS = !mode ? 60 : std::min(Config::TARGET_RENDER_FPS, static_cast<uint32_t>(mode->refresh_rate)); 

	m_currentFPS = 0;
	m_fpsAccumulatedTime = 0;
	m_fpsAccumulatedTime = 0;

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
	++m_fpsFrameCount;
	m_fpsAccumulatedTime += frameDt;

	if (m_fpsAccumulatedTime >= 0.25) {
		m_currentFPS = static_cast<int>(m_fpsFrameCount / m_fpsAccumulatedTime + 0.5);
		m_fpsFrameCount = 0;
		m_fpsAccumulatedTime = 0.0;
	}
}

