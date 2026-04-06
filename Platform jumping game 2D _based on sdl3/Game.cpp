#include "Game.h"
#include "Config.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "render/Renderer.h"
#include "resource/Resource.h"
#include "render/Animation.h"

Game::Game() {
	if (!init()) {
		SDL_Log("Failed to initialize game.");
		return;
	}
}

Game::~Game() {
	EventManager::getInstance().unsubscribe(quitSubscriptionId_);
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
		uiMananger_->update();
		renderer();

		SDL_Delay(12);// 简单的帧率控制，避免CPU占用过高，后续可以改进为更精确的帧率控制机制
	}
}

void Game::handleInput() noexcept {
	Input::getInstance().resetInputState();

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			isRunning_ = false;
			continue;
		}

		Input::getInstance().processInput(event);
	}

	uiMananger_->handleInput();
	EventManager::getInstance().update();
}

void Game::update() noexcept {
	while (accumulator_ >= Config::DELTAFREAM) {
		stateMachine_->update(Config::DELTAFREAM);
		GlobalTime_ += Config::DELTAFREAM;
		accumulator_ -= Config::DELTAFREAM;
	}
}

void Game::renderer() const noexcept {
	Renderer::getInstance().beginRender();

	stateMachine_->render();
	uiMananger_->render();
	//stateMachine_->render();

	Renderer::getInstance().restoreDefaultAndPresent();
}

bool Game::init() noexcept {
	if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
		return false;
	}

	if (!Renderer::getInstance().init(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT)) {
		SDL_Log("Failed to initialize renderer.");
		return false;
	}
	if (!EventManager::getInstance().init()) {
		SDL_Log("Failed to initialize event manager.");
		return false;
	}
	if (!Input::getInstance().init()) {
		SDL_Log("Failed to initialize input manager.");
		return false;
	}
	if(!Resource::getInstance().init()) {
		SDL_Log("Failed to initialize resource manager.");
		return false;
	}

	animation_ = std::make_unique<Animation>();
	if(!animation_->init()) {
		SDL_Log("Failed to initialize animation.");
		return false;
	}

	quitSubscriptionId_ = EventManager::getInstance().subscribe(EventType::App_Quit, [this](const Event&) {
		isRunning_ = false;
	});

	stateMachine_ = std::make_unique<GameStateMachine>(StateType::MENU, *animation_);
	uiMananger_ = std::make_unique<GameUIManager>(UIType::MENU);
	lastFrameTime_ = std::chrono::high_resolution_clock::now();
	return true;
}
