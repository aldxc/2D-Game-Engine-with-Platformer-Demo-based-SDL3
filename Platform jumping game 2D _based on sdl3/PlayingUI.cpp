#include "PlayingUI.h"
#include "core/EventManager.h"
#include "render/Renderer.h"
#include "input/Input.h"
#include "Config.h"
#include "StateType.h"


PlayingUI::PlayingUI() : UI(UIType::PLAYING){
	//test botton
	SDL_FRect bottonRect{ 0, 300, 100, 100 };
	bottons_[0] = { bottonRect, "Won", SDL_Color({ 100, 200, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[]() { EventManager::getInstance().sendEvent(Event{ EventType::State_Transition, StateType::WON }); },
		[]() { EventManager::getInstance().sendEvent(Event{ EventType::UI_Show, UIType::WON }); }
	} };
	SDL_FRect bottonRect2{ Config::WINDOW_WIDTH - 100, 300, 100, 100 };
	bottons_[1] = { bottonRect2, "Lose", SDL_Color({ 200, 100, 100, 255 }), Config::DEFAULT_TEXT_SIZE, {
		[]() { EventManager::getInstance().sendEvent(Event{ EventType::State_Transition, StateType::LOSE }); },
		[]() { EventManager::getInstance().sendEvent(Event{ EventType::UI_Show, UIType::LOSE }); }
	} };
}

PlayingUI::~PlayingUI(){

}

void PlayingUI::handleInput() noexcept{
	if (Input::getInstance().isMousePressed()) {
		auto [mouseX, mouseY] = Input::getInstance().getMousePosition();
		SDL_FPoint mousePoint{ mouseX, mouseY };
		for(const auto& botton : bottons_) {
			if (SDL_PointInRectFloat(&mousePoint, &botton.getRect())) {
				botton.clickBottom();
			 }
		}
	}
}

void PlayingUI::update() noexcept{
}

void PlayingUI::render() const noexcept{

	//test ÊÇ·ñ±»¸²¸Ç
	Renderer::getInstance().renderFillRect(SDL_FRect{ 0, 0, 200, 200 }, SDL_Color({ 0, 0, 255, 255 }));

	for(const auto& botton : bottons_) {
		botton.render();
	}
}
