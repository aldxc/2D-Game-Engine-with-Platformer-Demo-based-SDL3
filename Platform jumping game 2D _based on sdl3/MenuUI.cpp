#include <vector>
#include "MenuUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "StateType.h"
#include "UIType.h"
#include "GameSession.h"
#include "AudioId.h"

class Renderer;

MenuUI::MenuUI(Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept : UI(UIType::MENU), m_inputManager(iM), m_eventManager(eM), m_renderer(r), m_gameSession(gS) {

	m_eventManager.sendEvent({ EventType::AUDIO_PLAY_BGM, BgmId::MENU });

    // 初始化按钮
    SDL_FRect startRect{ (Config::LOGIC_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 100, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	m_bottons[0] = { startRect, "Start", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, 
		// 点击 Start 按钮的回调
		{ [this]() {
			m_eventManager.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::SELECT_HERO} });
			m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::SELECT_STATE });
			m_gameSession.reset();
		}
		}
	};

	SDL_FRect continueRect{ (Config::LOGIC_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 200, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	m_bottons[1] = { continueRect, "Continue", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, {} };// 暂未实现继续功能

	SDL_FRect settingsRect{ (Config::LOGIC_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 300, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	m_bottons[2] = { settingsRect, "Settings", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE, {} };// 暂未实现设置功能

	SDL_FRect quitRect{ (Config::LOGIC_WIDTH - Config::MENU_BUTTON_WIDTH) / 2, 400, Config::MENU_BUTTON_WIDTH, Config::MENU_BUTTON_HEIGHT };
	m_bottons[3] = { quitRect, "Quit", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		// 点击 Quit 按钮的回调
		{ [this]() {
			m_eventManager.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			m_eventManager.sendEvent(Event{ EventType::APP_QUIT });}
	} };

}

MenuUI::~MenuUI() noexcept {

}

void MenuUI::handleInput() noexcept {
	if (m_inputManager.isMousePressed()) {
		auto [mouseX, mouseY] = m_inputManager.getMousePosition();
		SDL_FPoint mousePoint{ mouseX, mouseY };
		for(const auto& botton : m_bottons) {
			if (SDL_PointInRectFloat(&mousePoint, &botton.getRect())) {
				botton.clickBottom();
                m_inputManager.consumeMousePress();
				break;
			}
		}
	}
}

void MenuUI::update(double dt) noexcept {

}

void MenuUI::render() const noexcept {
	for (const auto& botton : m_bottons) {
		botton.render(m_renderer);
	}
}
