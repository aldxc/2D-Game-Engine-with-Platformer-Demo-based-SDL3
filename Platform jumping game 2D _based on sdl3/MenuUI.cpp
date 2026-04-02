#include "MenuUI.h"
#include "EventManager.h"
#include "Input.h"

MenuUI::MenuUI() : UI(UIType::MENU) {
    // 初始化按钮
    SDL_FRect startRect{ (Config::WINDOW_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 100, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	bottons_[0] = { startRect, "Start", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, []() {
        // 点击 Start 按钮的回调
        EventManager::getInstance().sendEvent(Event{ EventType::State_Transition, StateType::PLAYING });
	} };

	SDL_FRect continueRect{ (Config::WINDOW_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 200, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	bottons_[1] = { continueRect, "Continue", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, nullptr };// 暂未实现继续功能

	SDL_FRect settingsRect{ (Config::WINDOW_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 300, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	bottons_[2] = { settingsRect, "Settings", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, nullptr };// 暂未实现设置功能

	SDL_FRect quitRect{ (Config::WINDOW_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 400, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	bottons_[3] = { quitRect, "Quit", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, []() {
		// 点击 Quit 按钮的回调
		EventManager::getInstance().sendEvent(Event{ EventType::App_Quit });
	} };
}

MenuUI::~MenuUI() {

}

void MenuUI::handleInput() noexcept {
	if (Input::getInstance().isMousePressed()) {
		auto [mouseX, mouseY] = Input::getInstance().getMousePosition();
		SDL_FPoint mousePoint{ mouseX, mouseY };
		for(auto& botton : bottons_) {
			if (SDL_PointInRectFloat(&mousePoint, &botton.getRect())) {
				botton.clickBottom();
			 }
		}
	}
}

void MenuUI::update() noexcept {
}

void MenuUI::render() const noexcept {
	for (const auto& botton : bottons_) {
		botton.render();
	}
}
