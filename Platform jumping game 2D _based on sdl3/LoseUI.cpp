#include "LoseUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "StateType.h"
#include "AudioId.h"

class Renderer;

LoseUI::LoseUI(Input& iM, EventManager& eM, Renderer& r) noexcept : UI<UIType>(UIType::LOSE), inputManager_(iM),eventManager_(eM), renderer_(r) {
	// 进入lose界面时停止背景音乐
	eventManager_.sendEvent(Event{ EventType::AUDIO_STOP_BGM }); 

	SDL_FRect showRect{ (Config::LOGIC_WIDTH - Config::LOSE_BUTTON_WIDTH) / 2, 100, Config::LOSE_BUTTON_WIDTH, Config::LOSE_BUTTON_HEIGHT };
	bottons_[0] = { showRect, "You LOSE!", SDL_Color({ 200, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE + 10, {} };
	SDL_FRect bottonRect{ (Config::LOGIC_WIDTH - Config::LOSE_BUTTON_WIDTH) / 2, 300, Config::LOSE_BUTTON_WIDTH, Config::LOSE_BUTTON_HEIGHT };
	bottons_[1] = { bottonRect, "Restart", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			eventManager_.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			eventManager_.sendEvent(Event{ EventType::AUDIO_STOP_BGM });
			eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::PLAYING} });
		    eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::PLAYING }); 
		} 
	}
	};
	SDL_FRect menuBottonRect{ (Config::LOGIC_WIDTH - Config::LOSE_BUTTON_WIDTH) / 2, 400, Config::LOSE_BUTTON_WIDTH, Config::LOSE_BUTTON_HEIGHT };
	bottons_[2] = { menuBottonRect, "Back to MENU", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			eventManager_.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::MENU} });
		    eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::MENU }); 
		} 
	}
	};
}

LoseUI::~LoseUI() noexcept {

}

void LoseUI::handleInput() noexcept {
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

void LoseUI::update(double dt) noexcept {

}

void LoseUI::render() const noexcept {
	for(const auto& botton : bottons_) {
		botton.render(renderer_);
	}
}
