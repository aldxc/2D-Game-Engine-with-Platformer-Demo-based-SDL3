#include "Input.h"

bool Input::init() noexcept {
	keyBindings_[SDL_SCANCODE_A] = InputAction::MOVE_LEFT;
	keyBindings_[SDL_SCANCODE_D] = InputAction::MOVE_RIGHT;
	keyBindings_[SDL_SCANCODE_SPACE] = InputAction::JUMP;
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
		isKeyPressed_ = true;
		switch (keyBindings_[event.key.scancode]) {
			case InputAction::MOVE_LEFT:
				isMoveLeftPressed_ = true;
				break;
			case InputAction::MOVE_RIGHT:
				isMoveRightPressed_ = true;
				break;
			case InputAction::JUMP:
				isJumpPressed_ = true;
				break;
			default:
				break;
		}
		break;
	default:
		break;
	}
}

void Input::resetInputState() noexcept{
	isKeyPressed_ = false;
	isMousePressed_ = false;
	mousePos_ = { 0, 0 };
	isMoveLeftPressed_ = false;
	isMoveRightPressed_ = false;
}
