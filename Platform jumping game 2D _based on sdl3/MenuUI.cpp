#include <vector>
#include "MenuUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "StateType.h"
#include "UIType.h"
#include "GameSession.h"
#include "AudioId.h"

class Renderer;

MenuUI::MenuUI(Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept : UI(UIType::MENU), inputManager_(iM), eventManager_(eM), renderer_(r), gameSession_(gS) {

	eventManager_.sendEvent({ EventType::AUDIO_PLAY_BGM, BgmId::MENU });

    // 初始化按钮
    SDL_FRect startRect{ (Config::LOGIC_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 100, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	bottons_[0] = { startRect, "Start", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, 
		// 点击 Start 按钮的回调
		{ [this]() {
			eventManager_.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::SELECT_HERO} });
			eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::SELECT_STATE });
			gameSession_.reset();
		}
		}
	};

	SDL_FRect continueRect{ (Config::LOGIC_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 200, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	bottons_[1] = { continueRect, "Continue", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, {} };// 暂未实现继续功能

	SDL_FRect settingsRect{ (Config::LOGIC_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 300, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	bottons_[2] = { settingsRect, "Settings", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, {} };// 暂未实现设置功能

	SDL_FRect quitRect{ (Config::LOGIC_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 400, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	bottons_[3] = { quitRect, "Quit", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		// 点击 Quit 按钮的回调
		{ [this]() {
			eventManager_.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			eventManager_.sendEvent(Event{ EventType::APP_QUIT });}
	} };

}

MenuUI::~MenuUI() noexcept {

}

void MenuUI::handleInput() noexcept {
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

void MenuUI::update(double dt) noexcept {

}

void MenuUI::render() const noexcept {
	for (const auto& botton : bottons_) {
		botton.render(renderer_);
	}
}
