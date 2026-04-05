#include "LoseUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "render/Renderer.h"
#include "StateType.h"

LoseUI::LoseUI() : UI<UIType>(UIType::LOSE) {
	SDL_FRect showRect{ (Config::WINDOW_WIDTH - Config::LOSE_BUTTON_WIDTH) / 2, 100, Config::LOSE_BUTTON_WIDTH, Config::LOSE_BUTTON_HEIGHT };
	bottons_[0] = { showRect, "You Lose!", SDL_Color({ 200, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE + 10, {} };
	SDL_FRect bottonRect{ (Config::WINDOW_WIDTH - Config::LOSE_BUTTON_WIDTH) / 2, 300, Config::LOSE_BUTTON_WIDTH, Config::LOSE_BUTTON_HEIGHT };
	bottons_[1] = { bottonRect, "Back to Menu", SDL_Color({ 200, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[]() { EventManager::getInstance().sendEvent(Event{ EventType::State_Transition, StateType::MENU }); },
		[]() { EventManager::getInstance().sendEvent(Event{ EventType::UI_Show, UIType::MENU }); }
	} };
}

LoseUI::~LoseUI() {

}

void LoseUI::handleInput() noexcept {
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

void LoseUI::update() noexcept {

}

void LoseUI::render() const noexcept {
	for(const auto& botton : bottons_) {
		botton.render();
	}
}
