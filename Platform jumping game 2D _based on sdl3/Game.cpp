#include "Game.h"
#include "Renderer.h"

Game::Game(){
	if (!init()) {
		SDL_Log("Failed to initialize game.");
		return;
	}
}

void Game::Run() noexcept{
	while (isRunning_) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				isRunning_ = false;
			}
			stateMachine_->handleInput();
		}

		//更新游戏状态，后续增加固定更新频率
		stateMachine_->update();


		stateMachine_->render();
	}
}

bool Game::init() noexcept{
	if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
		return false;
	}
	//包含所有单例的初始化，后续增加事件管理器的初始化
	//状态机的初始化
	stateMachine_ = std::make_unique<StateMachine>();
	return true;
}
