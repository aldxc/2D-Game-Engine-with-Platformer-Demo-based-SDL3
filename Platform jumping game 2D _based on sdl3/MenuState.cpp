#include "MenuState.h"
#include "Renderer.h"

MenuState::MenuState() noexcept : State(StateType::MENU){

}

void MenuState::update(float dt) noexcept{

}

void MenuState::render() const noexcept{
	//test
	SDL_FRect rect{ 0,0,100,100 };
	Renderer::getInstance().renderRect(rect, SDL_Color(100, 100, 100, 255));
}
