#pragma once
#include <utility>
#include <SDL3/SDL.h>

enum class InputAction { JUMP };//后续增加更多输入动作，如移动、攻击等

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

	void resetInputState() noexcept; // 重置输入状态，通常在每帧开始时调用
private:
	bool isKeyPressed_; // 键盘状态
	bool isMousePressed_; // 鼠标状态
	std::pair<float, float> mousePos_;
};