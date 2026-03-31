#pragma once

enum class StateType { MENU, PLAYING, PAUSE, WON, LOST};

class State {
public:
	State(StateType init = StateType::MENU) noexcept : type_(init) {}
	virtual ~State() = default;

	//禁用拷贝
	State(const State&) = delete;
	State& operator=(const State&) = delete;
	State(State&&) = delete;
	State& operator=(State&&) = delete;

	virtual void update(float dt) noexcept = 0;
	virtual void render() const noexcept = 0;
	bool isType(StateType type) const noexcept { return type_ == type; } // 比较接口
	StateType getType() const noexcept { return type_; } // 获取状态类型接口
private:
	StateType type_;
};