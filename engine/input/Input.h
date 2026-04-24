#pragma once
#include <utility>
#include <unordered_map>
#include <SDL3/SDL.h>

class Renderer;

enum class InputAction { JUMP, MOVE_LEFT, MOVE_RIGHT, ATTACK, UP, DOWN, SPRINT, FLASH, PAUSE };

class Input {
public:
	explicit Input() noexcept = default;
	~Input() noexcept = default;

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(Input&&) = delete;

	// 初始化输入系统，设置初始状态
	bool init() noexcept; 

	// 读取输入状态，更新内部状态
	void processInput(const SDL_Event& event, Renderer& renderer) noexcept; 

	// 提供接口供外界查询输入状态
	bool isKeyPressed() const noexcept { return m_isKeyPressed; } 
	bool isMousePressed() const noexcept { return m_isMousePressed; } 

	// 获取输入状态，例如鼠标位置、按键状态等
	std::pair<float, float> getMousePosition() const noexcept { return m_mousePos; }; 
	bool getMoveLeftPressed() const noexcept { return m_isMoveLeftPressed; } 
	bool getMoveRightPressed() const noexcept { return m_isMoveRightPressed; } 
	bool getJumpPressed() const noexcept { return m_isJumpPressed; }
	bool getAttackPressed() const noexcept { return m_isAttackPressed; } 
	bool getUPPressed() const noexcept { return m_isUPPressed; } 
	bool getDownPressed() const noexcept { return m_isDownPressed; } 
	bool getSprintPressed() const noexcept { return m_isSprintPressed; } 
	bool getFlashPressed() const noexcept { return m_isFlashPressed; } 
	bool getESCPressed() const noexcept { return m_isESCPressed; } 
	void consumeMousePress() noexcept { m_isMousePressed = false; }
	void consumeESCKeyPress() noexcept { m_isESCPressed = false; }

	// 重置输入状态
	void resetInputState() noexcept; 
private:
	bool m_isKeyPressed = false; 
	bool m_isMousePressed = false; 
	bool m_isMoveLeftPressed = false;
	bool m_isMoveRightPressed = false;
	bool m_isJumpPressed = false; 
	bool m_isAttackPressed = false; 
	bool m_isUPPressed = false; 
	bool m_isDownPressed = false; 
	bool m_isSprintPressed = false; 
	bool m_isFlashPressed = false; 
	bool m_isESCPressed = false; 
	std::pair<float, float> m_mousePos = {};
	// 键盘绑定，后续支持自定义按键绑定
	std::unordered_map<SDL_Scancode, InputAction> m_keyBindings; 
};