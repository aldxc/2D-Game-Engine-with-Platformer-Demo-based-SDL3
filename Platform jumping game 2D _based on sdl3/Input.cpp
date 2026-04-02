#include "Input.h"

bool Input::init() noexcept {
	resetInputState();
	return true;
}

void Input::processInput(const SDL_Event& event) noexcept{
	switch (event.type){
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		isMousePressed_ = true;
		mousePos_ = { event.button.x, event.button.y };
		break;
	case SDL_EVENT_KEY_DOWN:
		break;
	default:
		break;
	}
}

void Input::resetInputState() noexcept{
	isKeyPressed_ = false;
	isMousePressed_ = false;
	mousePos_ = { 0, 0 };
}
