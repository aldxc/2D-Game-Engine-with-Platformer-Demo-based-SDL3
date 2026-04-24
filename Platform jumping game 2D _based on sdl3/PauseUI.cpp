#include "PauseUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "StateType.h"
#include "render/Renderer.h"

PauseUI::PauseUI(Input& iM, EventManager& eM, Renderer& r) noexcept : UI<UIType>(UIType::PAUSE), m_inputManager(iM), m_eventManager(eM), m_renderer(r) {
	m_eventManager.sendEvent(Event{ EventType::AUDIO_PAUSE_BGM });

	Rect resumeRect = Rect{ (Config::LOGIC_WIDTH - Config::SELECT_BUTTON_WIDTH) / 2, 100, Config::SELECT_BUTTON_WIDTH, Config::SELECT_BUTTON_HEIGHT };
	m_bottons[0] = { resumeRect, "Resume", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			m_eventManager.sendEvent(Event{ EventType::AUDIO_RESUME_BGM });
			m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::PLAYING });
			m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::POP, {}} });
		} }
	};
	Rect restartRect = Rect{ (Config::LOGIC_WIDTH - Config::SELECT_BUTTON_WIDTH) / 2, 200, Config::SELECT_BUTTON_WIDTH, Config::SELECT_BUTTON_HEIGHT };
	m_bottons[1] = {restartRect, "Restart", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			m_eventManager.sendEvent(Event{EventType::AUDIO_STOP_BGM});
			m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::CLEAR_AND_PUSH, StateType::PLAYING} });
			m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::PLAYING });
		} }
	};
	Rect menuRect = Rect{ (Config::LOGIC_WIDTH - Config::SELECT_BUTTON_WIDTH) / 2, 300, Config::SELECT_BUTTON_WIDTH, Config::SELECT_BUTTON_HEIGHT };
	m_bottons[2] = { menuRect, "Back to MENU", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			m_eventManager.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::CLEAR_AND_PUSH, StateType::MENU} });
			m_eventManager.sendEvent(Event{ EventType::UI_SHOW, UIType::MENU });
		} }
	};
}

void PauseUI::handleInput() noexcept{
	if(m_inputManager.isMousePressed()) {
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

void PauseUI::update(double dt) noexcept{

}

void PauseUI::render() const noexcept{
	m_renderer.renderFillRect(
		Rect{ 0, 0, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT },
		m_renderer.setColorAlpha( 0, 0, 0, 180 )  // alpha=180£¬°ëÍ¸Ã÷
	);
	for(const auto& botton : m_bottons) {
		botton.render(m_renderer);
	}
}
