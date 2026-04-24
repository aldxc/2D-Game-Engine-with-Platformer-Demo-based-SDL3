#include "Input.h"
#include "render/Renderer.h"

bool Input::init() noexcept {
	// 默认键盘绑定
	m_keyBindings[SDL_SCANCODE_LEFT] = InputAction::MOVE_LEFT;
	m_keyBindings[SDL_SCANCODE_RIGHT] = InputAction::MOVE_RIGHT;
	m_keyBindings[SDL_SCANCODE_C] = InputAction::JUMP;
	m_keyBindings[SDL_SCANCODE_Z] = InputAction::ATTACK;
	m_keyBindings[SDL_SCANCODE_UP] = InputAction::UP;
	m_keyBindings[SDL_SCANCODE_DOWN] = InputAction::DOWN;
	m_keyBindings[SDL_SCANCODE_X] = InputAction::SPRINT;
	m_keyBindings[SDL_SCANCODE_ESCAPE] = InputAction::PAUSE;
	resetInputState();
	return true;
}

void Input::processInput(const SDL_Event& event, Renderer& renderer) noexcept{

	// 鼠标位置始终更新
	if (SDL_EVENT_MOUSE_MOTION) {
		float mouseX = 0.0f, mouseY = 0.0f;
		SDL_RenderCoordinatesFromWindow(renderer.getSDLRenderer(), event.button.x, event.button.y, &mouseX, &mouseY);
		m_mousePos = { mouseX, mouseY };
	}

	switch (event.type) {
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		m_isMousePressed = true;
		break;
	}
	case SDL_EVENT_KEY_DOWN: {
		// 忽略按键重复事件，确保输入状态只在按键首次按下的那一帧更新，避免持续按键导致的重复输入问题
		if (event.key.repeat) {
			break; 
		}

		m_isKeyPressed = true;
		const auto it = m_keyBindings.find(event.key.scancode);
		if (it == m_keyBindings.end()) {
			break; 
		}
		switch (it->second) {
		case InputAction::MOVE_LEFT:
			m_isMoveLeftPressed = true;
			break;
		case InputAction::MOVE_RIGHT:
			m_isMoveRightPressed = true;
			break;
		case InputAction::JUMP:
			m_isJumpPressed = true;
			break;
		case InputAction::ATTACK:
			m_isAttackPressed = true;
			break;
		case InputAction::UP:
			m_isUPPressed = true;
			break;
		case InputAction::DOWN:
			m_isDownPressed = true;
			break;
		case InputAction::SPRINT:
			m_isSprintPressed = true;
			break;
		case InputAction::PAUSE:
			m_isESCPressed = true;
			break;
		default:
			break;
		}
		break;
	}
	case SDL_EVENT_KEY_UP: {
		const auto it = m_keyBindings.find(event.key.scancode);
		if (it == m_keyBindings.end()) {
			break;
		}

		switch (it->second) {
		case InputAction::MOVE_LEFT: {
			m_isMoveLeftPressed = false;
			break;
		}
		case InputAction::MOVE_RIGHT: {
			m_isMoveRightPressed = false;
			break;
		}
		case InputAction::UP: {
			m_isUPPressed = false;
			break;
		}
		case InputAction::DOWN: {
			m_isDownPressed = false;
			break;
		}
		case InputAction::PAUSE: {
			m_isESCPressed = false;
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
	m_isKeyPressed = false;
	m_isMousePressed = false;
	m_isJumpPressed = false;
	m_isAttackPressed = false;
	m_isSprintPressed = false;
}
