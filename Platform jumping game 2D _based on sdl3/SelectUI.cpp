#include "SelectUI.h"
#include "StateType.h"
#include "input/Input.h"
#include "core/EventManager.h"
#include "render/Renderer.h"
#include "GameSession.h"
#include "AudioId.h"

SelectUI::SelectUI(Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept : UI<UIType>(UIType::SelectState), inputManager_(iM), eventManager_(eM), renderer_(r), gameSession_(gS){
	eventManager_.sendEvent({ EventType::Audio_PlayBgm, BgmId::Select });

	bottons_[0] = Botton(Config::SELECT_BUTTONS_RECT[0], "", SDL_Color({100, 100, 100, 255}), Config::DEFAULT_TEXT_SIZE, {
		[this]() {
			eventManager_.sendEvent(Event{EventType::Audio_PlaySfx, SfxId::UIButtonClick});
			eventManager_.sendEvent(Event{ EventType::State_Transition, StateRequest{StateOperator::Replace, StateType::PLAYING} });
			eventManager_.sendEvent(Event{ EventType::UI_Show, UIType::PLAYING });
			gameSession_.setPlayerSkinIndex(0);
		}
		});
	bottons_[1] = Botton(Config::SELECT_BUTTONS_RECT[1], "", SDL_Color({ 100, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
	[this]() {
		eventManager_.sendEvent(Event{EventType::Audio_PlaySfx, SfxId::UIButtonClick});
		eventManager_.sendEvent(Event{ EventType::State_Transition,StateRequest{StateOperator::Replace, StateType::PLAYING} });
		eventManager_.sendEvent(Event{ EventType::UI_Show, UIType::PLAYING });
		gameSession_.setPlayerSkinIndex(1);
		}
		});
	bottons_[2] = Botton(Config::SELECT_BUTTONS_RECT[2], "", SDL_Color({ 100, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
	[this]() {
		eventManager_.sendEvent(Event{EventType::Audio_PlaySfx, SfxId::UIButtonClick});
		eventManager_.sendEvent(Event{ EventType::State_Transition, StateRequest{StateOperator::Replace, StateType::PLAYING} });
		eventManager_.sendEvent(Event{ EventType::UI_Show, UIType::PLAYING });
		gameSession_.setPlayerSkinIndex(2);
		}
		});
	
}

SelectUI::~SelectUI() noexcept{

}

void SelectUI::handleInput() noexcept{
	if (inputManager_.isMousePressed()) {
		auto [mouseX, mouseY] = inputManager_.getMousePosition();
		SDL_FPoint mousePoint{ mouseX, mouseY };
		for (const auto& botton : bottons_) {
			if (SDL_PointInRectFloat(&mousePoint, &botton.getRect())) {
				botton.clickBottom();
                inputManager_.consumeMousePress();
				break;
			}
		}
	}
}

void SelectUI::update(double dt) noexcept{

}

void SelectUI::render() const noexcept{
	std::string str = "Select Your Hero";
	Rect rect = Rect((Config::LOGIC_WIDTH - Config::SELECT_BUTTONS_WIDTH) / 2, 50, Config::SELECT_BUTTONS_WIDTH, 50);
	renderer_.renderText(str, rect, renderer_.setColorAlpha(200, 100, 100, 255) , Config::DEFAULT_TEXT_SIZE + 10);
	for(const auto& botton : bottons_) {
		botton.render(renderer_);
	}
}
