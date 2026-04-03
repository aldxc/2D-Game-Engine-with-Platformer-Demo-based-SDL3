#pragma once
#include <utility>
#include <unordered_map>
#include <SDL3/SDL.h>


enum class InputAction { JUMP, MOVE_LEFT, MOVE_RIGHT };//后续增加更多输入动作，冲刺、攻击等

//最好单例
class Input {
private:
	Input() = default;
	~Input() = default;
public:
	static Input& getInstance() {
		static Input instance;
		return instance;
	}
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(Input&&) = delete;
	bool init() noexcept; // 初始化输入系统，设置初始状态
	void processInput(const SDL_Event& event) noexcept; // 读取输入状态，更新内部状态

	bool isKeyPressed() const noexcept { return isKeyPressed_; } // 示例：检查某个键是否被按下
	bool isMousePressed() const noexcept { return isMousePressed_; } // 示例：检查鼠标是否被按下

	std::pair<float, float> getMousePosition() const noexcept { return mousePos_; }; // 获取鼠标位置，返回(x, y)坐标
	bool getMoveLeftPressed() const noexcept { return isMoveLeftPressed_; } // 获取左移状态
	bool getMoveRightPressed() const noexcept { return isMoveRightPressed_; } // 获取右移状态
	bool getJumpPressed() const noexcept { return isJumpPressed_; } // 获取跳跃状态

	void resetInputState() noexcept; // 重置输入状态，通常在每帧开始时调用
private:
	bool isKeyPressed_ = false; // 键盘状态
	bool isMousePressed_ = false; // 鼠标状态
	bool isMoveLeftPressed_ = false; // 左移
	bool isMoveRightPressed_ = false; // 右移
	bool isJumpPressed_ = false; // 跳跃
	std::pair<float, float> mousePos_ = {};
	std::unordered_map<SDL_Scancode, InputAction> keyBindings_; // 键盘绑定，后续支持自定义按键绑定
};