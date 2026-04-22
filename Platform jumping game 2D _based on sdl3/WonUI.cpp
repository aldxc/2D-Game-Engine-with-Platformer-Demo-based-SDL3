#include "WonUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "StateType.h"
#include "AudioId.h"

class Renderer;

WonUI::WonUI(Input& iM, EventManager& eM, Renderer& r) noexcept : UI<UIType>(UIType::WON), inputManager_(iM), eventManager_(eM), renderer_(r)  {
	eventManager_.sendEvent(Event{ EventType::Audio_StopBgm }); // œ»‘›Õ£

	SDL_FRect showRect{ (Config::LOGIC_WIDTH - Config::WON_BUTTON_WIDTH) / 2, 100, Config::WON_BUTTON_WIDTH, Config::WON_BUTTON_HEIGHT };
	bottons_[0] = { showRect, "Pass the Level", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE , {} };
	SDL_FRect nextLevelRect{ (Config::LOGIC_WIDTH - Config::WON_BUTTON_WIDTH) / 2, 200, Config::WON_BUTTON_WIDTH, Config::WON_BUTTON_HEIGHT };
	bottons_[1] = { nextLevelRect, "Next Level", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			eventManager_.sendEvent(Event{EventType::Audio_PlaySfx, SfxId::UIButtonClick});
			eventManager_.sendEvent(Event{ EventType::State_Transition, StateRequest{StateOperator::Replace, StateType::PLAYING} });
			eventManager_.sendEvent(Event{ EventType::UI_Show, UIType::PLAYING });} 
		}
	};
	SDL_FRect backToMenuRect{ (Config::LOGIC_WIDTH - Config::WON_BUTTON_WIDTH) / 2, 300, Config::WON_BUTTON_WIDTH, Config::WON_BUTTON_HEIGHT };
	bottons_[2] = { backToMenuRect, "Back to Menu", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			eventManager_.sendEvent(Event{EventType::Audio_PlaySfx, SfxId::UIButtonClick});
			eventManager_.sendEvent(Event{ EventType::State_Transition, StateRequest{StateOperator::Replace, StateType::MENU} });
			eventManager_.sendEvent(Event{ EventType::UI_Show, UIType::MENU }); }
		}
	};
}

WonUI::~WonUI() noexcept {

}

void WonUI::handleInput() noexcept{
	if (inputManager_.isMousePressed()) {
		auto [mouseX, mouseY] = inputManager_.getMousePosition();
		SDL_FPoint mousePoint{ mouseX, mouseY };
		for(const auto& botton : bottons_) {
			if (SDL_PointInRectFloat(&mousePoint, &botton.getRect())) {
				botton.clickBottom();
                inputManager_.consumeMousePress();
				break;
			 }
		}
	}
}

void WonUI::update(double dt) noexcept{

}

void WonUI::render() const noexcept{
	for(const auto& botton : bottons_) {
		botton.render(renderer_);
	}
}
