#include "PauseUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "StateType.h"
#include "render/Renderer.h"

PauseUI::PauseUI(Input& iM, EventManager& eM, Renderer& r) noexcept : UI<UIType>(UIType::PAUSE), inputManager_(iM), eventManager_(eM), renderer_(r) {
	eventManager_.sendEvent(Event{ EventType::AUDIO_PAUSE_BGM });

	Rect resumeRect = Rect{ (Config::LOGIC_WIDTH - Config::SELECT_BUTTON_WIDTH) / 2, 100, Config::SELECT_BUTTON_WIDTH, Config::SELECT_BUTTON_HEIGHT };
	bottons_[0] = { resumeRect, "Resume", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			eventManager_.sendEvent(Event{ EventType::AUDIO_RESUME_BGM });
			eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::PLAYING });
			eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::POP, {}} });
		} }
	};
	Rect restartRect = Rect{ (Config::LOGIC_WIDTH - Config::SELECT_BUTTON_WIDTH) / 2, 200, Config::SELECT_BUTTON_WIDTH, Config::SELECT_BUTTON_HEIGHT };
	bottons_[1] = {restartRect, "Restart", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			eventManager_.sendEvent(Event{EventType::AUDIO_STOP_BGM});
			eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::CLEAR_AND_PUSH, StateType::PLAYING} });
			eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::PLAYING });
		} }
	};
	Rect menuRect = Rect{ (Config::LOGIC_WIDTH - Config::SELECT_BUTTON_WIDTH) / 2, 300, Config::SELECT_BUTTON_WIDTH, Config::SELECT_BUTTON_HEIGHT };
	bottons_[2] = { menuRect, "Back to MENU", SDL_Color({100, 200, 100, 255}), Config::DEFAULT_TEXT_SIZE,
		{ [this]() {
			eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::CLEAR_AND_PUSH, StateType::MENU} });
			eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::MENU });
		} }
	};
}

void PauseUI::handleInput() noexcept{
	if(inputManager_.isMousePressed()) {
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

void PauseUI::update(double dt) noexcept{

}

void PauseUI::render() const noexcept{
	renderer_.renderFillRect(
		Rect{ 0, 0, Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT },
		renderer_.setColorAlpha( 0, 0, 0, 180 )  // alpha=180£¬°ëÍ¸Ã÷
	);
	for(const auto& botton : bottons_) {
		botton.render(renderer_);
	}
}
