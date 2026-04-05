#include "WonUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "render/Renderer.h"
#include "StateType.h"

WonUI::WonUI() : UI<UIType>(UIType::WON){
	SDL_FRect showRect{ (Config::WINDOW_WIDTH - Config::WON_BUTTON_WIDTH) / 2, 100, Config::WON_BUTTON_WIDTH, Config::WON_BUTTON_HEIGHT };
	bottons_[0] = { showRect, "Pass the Level", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE , {} };
	SDL_FRect nextLevelRect{ (Config::WINDOW_WIDTH - Config::WON_BUTTON_WIDTH) / 2, 200, Config::WON_BUTTON_WIDTH, Config::WON_BUTTON_HEIGHT };
	bottons_[1] = { nextLevelRect, "Next Level", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ []() {EventManager::getInstance().sendEvent(Event{ EventType::State_Transition, StateType::PLAYING });},
		  []() {EventManager::getInstance().sendEvent(Event{ EventType::UI_Show, UIType::PLAYING });} }
	};
	SDL_FRect backToMenuRect{ (Config::WINDOW_WIDTH - Config::WON_BUTTON_WIDTH) / 2, 300, Config::WON_BUTTON_WIDTH, Config::WON_BUTTON_HEIGHT };
	bottons_[2] = { backToMenuRect, "Back to Menu", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ []() {EventManager::getInstance().sendEvent(Event{ EventType::State_Transition, StateType::MENU });},
		  []() {EventManager::getInstance().sendEvent(Event{ EventType::UI_Show, UIType::MENU });} }
	};
}

WonUI::~WonUI(){

}

void WonUI::handleInput() noexcept{
	if (Input::getInstance().isMousePressed()) {
		auto [mouseX, mouseY] = Input::getInstance().getMousePosition();
		SDL_FPoint mousePoint{ mouseX, mouseY };
		for(const auto& botton : bottons_) {
			if (SDL_PointInRectFloat(&mousePoint, &botton.getRect())) {
				botton.clickBottom();
			 }
		}
	}
}

void WonUI::update() noexcept{

}

void WonUI::render() const noexcept{
	for(const auto& botton : bottons_) {
		botton.render();
	}
}
