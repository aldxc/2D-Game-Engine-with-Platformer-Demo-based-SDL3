#include "PlayingUI.h"
#include "core/EventManager.h"
#include "input/Input.h"
#include "Config.h"
#include "StateType.h"
#include "AudioId.h"

class Renderer;

PlayingUI::PlayingUI(Input& iM, EventManager& eM, Renderer& r) noexcept : UI(UIType::PLAYING), inputManager_(iM), eventManager_(eM), renderer_(r){
	eventManager_.sendEvent({ EventType::AUDIO_PLAY_BGM, BgmId::PLAYING });
	//test botton
	SDL_FRect bottonRect{ 0, 300, 100, 100 };
	bottons_[0] = { bottonRect, "Won", SDL_Color({ 100, 200, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[this]() { 
			eventManager_.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::REPLACE, StateType::WON} }); },
		[this]() { eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::WON }); }
	} };
	SDL_FRect bottonRect2{ Config::LOGIC_WIDTH - 100, 300, 100, 100 };
	bottons_[1] = { bottonRect2, "LOSE", SDL_Color({ 200, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[this]() { 
			eventManager_.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION,StateRequest{StateOperator::REPLACE, StateType::LOSE} }); },
		[this]() { eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::LOSE }); }
	} };
	SDL_FRect debugBottonRect{ Config::LOGIC_WIDTH / 2 - 50, 0, 100, 100 };
	bottons_[2] = { debugBottonRect, "Debug", SDL_Color({ 255, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[this]() { 
			eventManager_.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			eventManager_.sendEvent(Event{EventType::DEBUG_TOGGLE_PLAYER_INFO, {}}); }
	} };
	Rect pauseBottonRect{ Config::LOGIC_WIDTH - 100, 0, 100, 100 };
	bottons_[3] = { pauseBottonRect, "Pause", SDL_Color({ 255, 255, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[this]() { 
			eventManager_.sendEvent(Event{EventType::AUDIO_PLAY_SFX, SfxId::UI_BUTTON_CLICK});
			eventManager_.sendEvent(Event{ EventType::STATE_TRANSITION, StateRequest{StateOperator::PUSH, StateType::PAUSE} }); },
		[this]() { eventManager_.sendEvent(Event{ EventType::UI_SHOW, UIType::PAUSE }); }
	} };

}

PlayingUI::~PlayingUI() noexcept {

}

void PlayingUI::handleInput() noexcept{
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

void PlayingUI::update(double dt) noexcept{
}

void PlayingUI::render() const noexcept{
	for(const auto& botton : bottons_) {
		botton.render(renderer_);
	}
}
