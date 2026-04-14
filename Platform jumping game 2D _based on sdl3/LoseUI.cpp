#include "LoseUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "StateType.h"

class Renderer;

LoseUI::LoseUI(Input& iM, EventManager& eM, Renderer& r) : UI<UIType>(UIType::LOSE), inputManager_(iM),eventManager_(eM), renderer_(r) {
	SDL_FRect showRect{ (Config::LOGIC_WIDTH - Config::LOSE_BUTTON_WIDTH) / 2, 100, Config::LOSE_BUTTON_WIDTH, Config::LOSE_BUTTON_HEIGHT };
	bottons_[0] = { showRect, "You Lose!", SDL_Color({ 200, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE + 10, {} };
	SDL_FRect bottonRect{ (Config::LOGIC_WIDTH - Config::LOSE_BUTTON_WIDTH) / 2, 300, Config::LOSE_BUTTON_WIDTH, Config::LOSE_BUTTON_HEIGHT };
	bottons_[1] = { bottonRect, "Back to Menu", SDL_Color({ 200, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[this]() { eventManager_.sendEvent(Event{ EventType::State_Transition, StateType::MENU }); },
		[this]() { eventManager_.sendEvent(Event{ EventType::UI_Show, UIType::MENU }); }
	} };
}

LoseUI::~LoseUI() {

}

void LoseUI::handleInput() noexcept {
	if (inputManager_.isMousePressed()) {
		auto [mouseX, mouseY] = inputManager_.getMousePosition();
		SDL_FPoint mousePoint{ mouseX, mouseY };
		for(const auto& botton : bottons_) {
			if (SDL_PointInRectFloat(&mousePoint, &botton.getRect())) {
				botton.clickBottom();
			 }
		}
	}
}

void LoseUI::update(float dt) noexcept {

}

void LoseUI::render() const noexcept {
	for(const auto& botton : bottons_) {
		botton.render(renderer_);
	}
}
