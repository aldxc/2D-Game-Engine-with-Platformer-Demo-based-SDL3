#include "Input.h"
#include "render/Renderer.h"

bool Input::init() noexcept {
	// 默认键盘绑定
	keyBindings_[SDL_SCANCODE_LEFT] = InputAction::MOVE_LEFT;
	keyBindings_[SDL_SCANCODE_RIGHT] = InputAction::MOVE_RIGHT;
	keyBindings_[SDL_SCANCODE_C] = InputAction::JUMP;
	keyBindings_[SDL_SCANCODE_Z] = InputAction::ATTACK;
	keyBindings_[SDL_SCANCODE_UP] = InputAction::UP;
	keyBindings_[SDL_SCANCODE_DOWN] = InputAction::DOWN;
	keyBindings_[SDL_SCANCODE_X] = InputAction::SPRINT;
	keyBindings_[SDL_SCANCODE_ESCAPE] = InputAction::PAUSE;
	resetInputState();
	return true;
}

void Input::processInput(const SDL_Event& event, Renderer& renderer) noexcept{

	// 鼠标位置始终更新
	if (SDL_EVENT_MOUSE_MOTION) {
		float mouseX = 0.0f, mouseY = 0.0f;
		SDL_RenderCoordinatesFromWindow(renderer.getSDLRenderer(), event.button.x, event.button.y, &mouseX, &mouseY);
		mousePos_ = { mouseX, mouseY };
	}

	switch (event.type) {
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		isMousePressed_ = true;
		break;
	}
	case SDL_EVENT_KEY_DOWN: {
		// 忽略按键重复事件，确保输入状态只在按键首次按下的那一帧更新，避免持续按键导致的重复输入问题
		if (event.key.repeat) {
			break; 
		}

		isKeyPressed_ = true;
		const auto it = keyBindings_.find(event.key.scancode);
		if (it == keyBindings_.end()) {
			break; 
		}
		switch (it->second) {
		case InputAction::MOVE_LEFT:
			isMoveLeftPressed_ = true;
			break;
		case InputAction::MOVE_RIGHT:
			isMoveRightPressed_ = true;
			break;
		case InputAction::JUMP:
			isJumpPressed_ = true;
			break;
		case InputAction::ATTACK:
			isAttackPressed_ = true;
			break;
		case InputAction::UP:
			isUPPressed_ = true;
			break;
		case InputAction::DOWN:
			isDownPressed_ = true;
			break;
		case InputAction::SPRINT:
			isSprintPressed_ = true;
			break;
		case InputAction::PAUSE:
			isESCPressed_ = true;
			break;
		default:
			break;
		}
		break;
	}
	case SDL_EVENT_KEY_UP: {
		const auto it = keyBindings_.find(event.key.scancode);
		if (it == keyBindings_.end()) {
			break;
		}

		switch (it->second) {
		case InputAction::MOVE_LEFT: {
			isMoveLeftPressed_ = false;
			break;
		}
		case InputAction::MOVE_RIGHT: {
			isMoveRightPressed_ = false;
			break;
		}
		case InputAction::UP: {
			isUPPressed_ = false;
			break;
		}
		case InputAction::DOWN: {
			isDownPressed_ = false;
			break;
		}
		case InputAction::PAUSE: {
			isESCPressed_ = false;
			break;
		}
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}

void Input::resetInputState() noexcept{
	isKeyPressed_ = false;
	isMousePressed_ = false;
	isJumpPressed_ = false;
	isAttackPressed_ = false;
	isSprintPressed_ = false;
}
