#include "PlayingUI.h"
#include "Renderer.h"



PlayingUI::PlayingUI() : UI(UIType::PLAYING){

}

PlayingUI::~PlayingUI(){

}

void PlayingUI::handleInput() noexcept{
}

void PlayingUI::update() noexcept{
}

void PlayingUI::render() const noexcept{
	Renderer::getInstance().renderText("Playing...", SDL_FRect{ 50, 50, 200, 50 }, SDL_Color({ 100, 100, 255, 255 }), Config::DEFAULT_TEXT_SIZE);
}
