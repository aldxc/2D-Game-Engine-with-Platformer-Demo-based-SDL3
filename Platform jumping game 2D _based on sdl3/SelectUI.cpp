#include "SelectUI.h"
#include "StateType.h"
#include "input/Input.h"
#include "core/EventManager.h"
#include "render/Renderer.h"
#include "GameSession.h"
#include "AudioId.h"

SelectUI::SelectUI(Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept : UI<UIType>(UIType::SELECT_STATE), m_inputManager(iM), m_eventManager(eM), m_renderer(r), m_gameSession(gS){
	m_eventManager.sendEvent({ EventType::AUDIO_PLAY_BGM, BgmId::SELECT });

	m_bottons[0] = Botton(Config::SELECT_BUTTONS_RECT[0], "", SDL_Color({100, 100, 100, 255}), Config::DEFAULT_TEXT_SIZE, {
		[this]() {
			m_eventManager.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::PLAYING} });
			m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::PLAYING });
			m_gameSession.setPlayerSkinIndex(0);
		}
		});
	m_bottons[1] = Botton(Config::SELECT_BUTTONS_RECT[1], "", SDL_Color({ 100, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
	[this]() {
		m_eventManager.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
		m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION,StateRequest{StateOperator::REPLACE, StateType::PLAYING} });
		m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::PLAYING });
		m_gameSession.setPlayerSkinIndex(1);
		}
		});
	m_bottons[2] = Botton(Config::SELECT_BUTTONS_RECT[2], "", SDL_Color({ 100, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
	[this]() {
		m_eventManager.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
		m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::PLAYING} });
		m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::PLAYING });
		m_gameSession.setPlayerSkinIndex(2);
		}
		});
	
}

SelectUI::~SelectUI() noexcept{

}

void SelectUI::handleInput() noexcept{
	if (m_inputManager.isMousePressed()) {
		auto [mouseX, mouseY] = m_inputManager.getMousePosition();
		SDL_FPoint mousePoint{ mouseX, mouseY };
		for (const auto& botton : m_bottons) {
			if (SDL_PointInRectFloat(&mousePoint, &botton.getRect())) {
				botton.clickBottom();
                m_inputManager.consumeMousePress();
				break;
			}
		}
	}
}

void SelectUI::update(double dt) noexcept{

}

void SelectUI::render() const noexcept{
	std::string str = "SELECT Your Hero";
	Rect rect = Rect((Config::LOGIC_WIDTH - Config::SELECT_BUTTONS_WIDTH) / 2, 50, Config::SELECT_BUTTONS_WIDTH, 50);
	m_renderer.renderText(str, rect, m_renderer.setColorAlpha(200, 100, 100, 255) , Config::DEFAULT_TEXT_SIZE + 10);
	for(const auto& botton : m_bottons) {
		botton.render(m_renderer);
	}
}
