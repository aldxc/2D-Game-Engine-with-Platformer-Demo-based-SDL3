#include "Game.h"
#include "Renderer.h"
#include "EventManager.h"
#include "Input.h"

Game::Game(){
	if (!init()) {
		SDL_Log("Failed to initialize game.");
		return;
	}
}

Game::~Game(){
	EventManager::getInstance().unsubscribe(quitSubscriptionId_);

}

void Game::Run() noexcept{
	while (isRunning_) {
		Input::getInstance().resetInputState(); // 每帧开始时重置输入状态

		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				isRunning_ = false;
			}
			Input::getInstance().processInput(event);
		}

		//更新游戏状态，后续增加固定更新频率

		uiMananger_->handleInput();
		EventManager::getInstance().update(); // 处理事件队列，调用回调函数
		stateMachine_->update();
		uiMananger_->update();

		renderer();
	}
}

void Game::renderer() const noexcept{
	Renderer::getInstance().beginRender();

	stateMachine_->render();
	uiMananger_->render();

	Renderer::getInstance().restoreDefaultAndPresent();
}

bool Game::init() noexcept{
	//注意各系统的初始化顺序
	if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
		return false;
	}

	//包含所有单例的初始化，后续增加事件管理器的初始化
	if (!Renderer::getInstance().init()) {//渲染器的初始化
		SDL_Log("Failed to initialize renderer.");
		return false;
	}
	if(!EventManager::getInstance().init()) {
		SDL_Log("Failed to initialize event manager.");
		return false;
	}
	if (!Input::getInstance().init()) {
		SDL_Log("Failed to initialize input manager.");
		return false;
	}

	quitSubscriptionId_ = EventManager::getInstance().subscribe(EventType::App_Quit, [this](const Event& event) {
		isRunning_ = false;
		});

	//状态机的初始化
	stateMachine_ = std::make_unique<StateMachine>();
	uiMananger_ = std::make_unique<UIManager>();
	return true;
}
