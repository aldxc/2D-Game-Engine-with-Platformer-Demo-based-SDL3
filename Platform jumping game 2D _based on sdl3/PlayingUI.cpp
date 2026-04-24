#include "PlayingUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "Config.h"
#include "StateType.h"
#include "AudioId.h"

class Renderer;

PlayingUI::PlayingUI(Input& iM, EventManager& eM, Renderer& r) noexcept : UI(UIType::PLAYING), m_inputManager(iM), m_eventManager(eM), m_renderer(r){
	m_eventManager.sendEvent({ EventType::AUDIO_PLAY_BGM, BgmId::PLAYING });
	//test botton
	SDL_FRect bottonRect{ 0, 300, 100, 100 };
	m_bottons[0] = { bottonRect, "Won", SDL_Color({ 100, 200, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[this]() { 
			m_eventManager.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::WON} }); },
		[this]() { m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::WON }); }
	} };
	SDL_FRect bottonRect2{ Config::LOGIC_WIDTH - 100, 300, 100, 100 };
	m_bottons[1] = { bottonRect2, "LOSE", SDL_Color({ 200, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[this]() { 
			m_eventManager.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION,StateRequest{StateOperator::REPLACE, StateType::LOSE} }); },
		[this]() { m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::LOSE }); }
	} };
	SDL_FRect debugBottonRect{ Config::LOGIC_WIDTH / 2 - 50, 0, 100, 100 };
	m_bottons[2] = { debugBottonRect, "Debug", SDL_Color({ 255, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[this]() { 
			m_eventManager.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			m_eventManager.sendEvent(Event{EventType::DEBUG_TOGGLE_PLAYER_INFO, {}}); }
	} };
	Rect pauseBottonRect{ Config::LOGIC_WIDTH - 100, 0, 100, 100 };
	m_bottons[3] = { pauseBottonRect, "Pause", SDL_Color({ 255, 255, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[this]() { 
			m_eventManager.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::PUSH, StateType::PAUSE} }); },
		[this]() { m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::PAUSE }); }
	} };

}

PlayingUI::~PlayingUI() noexcept {

}

void PlayingUI::handleInput() noexcept{
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

void PlayingUI::update(double dt) noexcept{
}

void PlayingUI::render() const noexcept{
	for(const auto& botton : m_bottons) {
		botton.render(m_renderer);
	}
}
