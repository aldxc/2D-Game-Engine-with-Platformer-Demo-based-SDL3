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
	bool isKeyPressed() const noexcept { return isKeyPressed_; } 
	bool isMousePressed() const noexcept { return isMousePressed_; } 

	// 获取输入状态，例如鼠标位置、按键状态等
	std::pair<float, float> getMousePosition() const noexcept { return mousePos_; }; 
	bool getMoveLeftPressed() const noexcept { return isMoveLeftPressed_; } 
	bool getMoveRightPressed() const noexcept { return isMoveRightPressed_; } 
	bool getJumpPressed() const noexcept { return isJumpPressed_; }
	bool getAttackPressed() const noexcept { return isAttackPressed_; } 
	bool getUPPressed() const noexcept { return isUPPressed_; } 
	bool getDownPressed() const noexcept { return isDownPressed_; } 
	bool getSprintPressed() const noexcept { return isSprintPressed_; } 
	bool getFlashPressed() const noexcept { return isFlashPressed_; } 
	bool getESCPressed() const noexcept { return isESCPressed_; } 
	void consumeMousePress() noexcept { isMousePressed_ = false; }
	void consumeESCKeyPress() noexcept { isESCPressed_ = false; }

	// 重置输入状态
	void resetInputState() noexcept; 
private:
	bool isKeyPressed_ = false; 
	bool isMousePressed_ = false; 
	bool isMoveLeftPressed_ = false;
	bool isMoveRightPressed_ = false;
	bool isJumpPressed_ = false; 
	bool isAttackPressed_ = false; 
	bool isUPPressed_ = false; 
	bool isDownPressed_ = false; 
	bool isSprintPressed_ = false; 
	bool isFlashPressed_ = false; 
	bool isESCPressed_ = false; 
	std::pair<float, float> mousePos_ = {};
	// 键盘绑定，后续支持自定义按键绑定
	std::unordered_map<SDL_Scancode, InputAction> keyBindings_; 
};