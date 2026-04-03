#include "MenuState.h"
#include "render/Renderer.h"

MenuState::MenuState() noexcept : State<StateType>(StateType::MENU){

}

void MenuState::update(float dt) noexcept{

}

void MenuState::render() const noexcept{
	//test
	//SDL_FRect rect{ 0,0,100,100 };
	//Renderer::getInstance().renderRect(rect, SDL_Color({ 200, 00, 100, 255 }));
}
