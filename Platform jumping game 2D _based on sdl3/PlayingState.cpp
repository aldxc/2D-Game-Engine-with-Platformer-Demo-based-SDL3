#include "PlayingState.h"
#include "Renderer.h"

PlayingState::PlayingState() noexcept : State(StateType::PLAYING) {
}

void PlayingState::render() const noexcept{
	//test
	SDL_FRect rect = { 50, 50, 200, 100 };
	Renderer::getInstance().renderText("Playing State", rect, SDL_Color({ 200, 100, 100, 255 }), 24);
}

void PlayingState::update(float dt) noexcept{

}
