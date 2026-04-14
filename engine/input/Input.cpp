#include "Input.h"
#include "render/Renderer.h"

bool Input::init() noexcept {
	keyBindings_[SDL_SCANCODE_LEFT] = InputAction::MOVE_LEFT;
	keyBindings_[SDL_SCANCODE_RIGHT] = InputAction::MOVE_RIGHT;
	keyBindings_[SDL_SCANCODE_C] = InputAction::JUMP;
	keyBindings_[SDL_SCANCODE_Z] = InputAction::ATTACK;
	keyBindings_[SDL_SCANCODE_UP] = InputAction::UP;
	keyBindings_[SDL_SCANCODE_DOWN] = InputAction::DOWN;
	keyBindings_[SDL_SCANCODE_X] = InputAction::SPRINT;
	resetInputState();
	return true;
}

void Input::processInput(const SDL_Event& event, Renderer& renderer) noexcept{
	switch (event.type) {
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		isMousePressed_ = true;
		float mouseX = 0.0f, mouseY = 0.0f;
		SDL_RenderCoordinatesFromWindow(renderer.getSDLRenderer(), event.button.x, event.button.y, &mouseX, &mouseY);
		//SDL_Log("MousePos,%f, %f", mouseX, mouseY);
		mousePos_ = { mouseX, mouseY };
		break;
	}
	case SDL_EVENT_KEY_DOWN: {
		if (event.key.repeat) {
			break; // 忽略按键重复事件，确保输入状态只在按键首次按下的那一帧更新，避免持续按键导致的重复输入问题
		}

		isKeyPressed_ = true;
		const auto it = keyBindings_.find(event.key.scancode);//不需要二次查找，直接使用迭代器访问绑定的动作，避免重复查找带来的性能问题
		if (it == keyBindings_.end()) {
			break; // 如果按键没有绑定任何动作，直接返回
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
		default:
			break;
		}
		break;
	}
	case SDL_EVENT_KEY_UP: {
		const auto it = keyBindings_.find(event.key.scancode);//不需要二次查找，直接使用迭代器访问绑定的动作，避免重复查找带来的性能问题
		if (it == keyBindings_.end()) {
			break; // 如果按键没有绑定任何动作，直接返回
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
	mousePos_ = { 0, 0 };
	isJumpPressed_ = false;// 跳跃状态通常在按键按下时设置，在每帧开始时重置，确保跳跃动作只在按键按下的那一帧触发
	isAttackPressed_ = false;
	isSprintPressed_ = false;
}
