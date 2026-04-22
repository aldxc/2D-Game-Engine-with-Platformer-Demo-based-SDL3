#pragma once
#include <utility>
#include <unordered_map>
#include <SDL3/SDL.h>

class Renderer;

enum class InputAction { JUMP, MOVE_LEFT, MOVE_RIGHT, ATTACK, UP, DOWN, SPRINT, FLASH, PAUSE };//后续增加更多输入动作，冲刺、攻击等

class Input {
public:
	explicit Input() noexcept = default;
	~Input() noexcept = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(Input&&) = delete;
	bool init() noexcept; // 初始化输入系统，设置初始状态
	void processInput(const SDL_Event& event, Renderer& renderer) noexcept; // 读取输入状态，更新内部状态

	bool isKeyPressed() const noexcept { return isKeyPressed_; } // 示例：检查某个键是否被按下
	bool isMousePressed() const noexcept { return isMousePressed_; } // 示例：检查鼠标是否被按下

	std::pair<float, float> getMousePosition() const noexcept { return mousePos_; }; // 获取鼠标位置，返回(x, y)坐标
	bool getMoveLeftPressed() const noexcept { return isMoveLeftPressed_; } // 获取左移状态
	bool getMoveRightPressed() const noexcept { return isMoveRightPressed_; } // 获取右移状态
	bool getJumpPressed() const noexcept { return isJumpPressed_; } // 获取跳跃状态
	bool getAttackPressed() const noexcept { return isAttackPressed_; } // 获取攻击状态
	bool getUPPressed() const noexcept { return isUPPressed_; } // 获取爬墙状态
	bool getDownPressed() const noexcept { return isDownPressed_; } // 获取下状态，例如下落、下蹲等
	bool getSprintPressed() const noexcept { return isSprintPressed_; } // 获取冲刺状态
	bool getFlashPressed() const noexcept { return isFlashPressed_; } // 获取闪现状态
	bool getESCPressed() const noexcept { return isESCPressed_; } // 获取ESC键状态
	void consumeMousePress() noexcept { isMousePressed_ = false; }
	void consumeESCKeyPress() noexcept { isESCPressed_ = false; }

	void resetInputState() noexcept; // 重置输入状态，通常在每帧开始时调用
private:
	bool isKeyPressed_ = false; // 键盘状态
	bool isMousePressed_ = false; // 鼠标状态
	bool isMoveLeftPressed_ = false; // 左移
	bool isMoveRightPressed_ = false; // 右移
	bool isJumpPressed_ = false; // 跳跃
	bool isAttackPressed_ = false; // 攻击
	bool isUPPressed_ = false; // 上
	bool isDownPressed_ = false; // 下
	bool isSprintPressed_ = false; // 冲刺
	bool isFlashPressed_ = false; // 闪现
	bool isESCPressed_ = false; // ESC键状态
	std::pair<float, float> mousePos_ = {};
	std::unordered_map<SDL_Scancode, InputAction> keyBindings_; // 键盘绑定，后续支持自定义按键绑定
};