#pragma once

// State.h - 定义游戏状态基类，使用CRTP模式实现状态类型的静态多态
template<class TStateType>
class State {
public:
	explicit State(TStateType init) noexcept : m_type(init) {}
	virtual ~State() = default;

	//禁用拷贝
	State(const State&) = delete;
	State& operator=(const State&) = delete;
	State(State&&) = delete;
	State& operator=(State&&) = delete;

	virtual void update(double dt) noexcept = 0;
	virtual void render() const noexcept = 0;
	// 比较接口
	bool isType(TStateType type) const noexcept { return m_type == type; } 
	// 获取状态类型接口
	TStateType getType() const noexcept { return m_type; } 

	// 钩子函数，允许在状态切换时执行特定操作，暂时未使用
	virtual void onEnter() noexcept {}
	virtual void onExit() noexcept {}
	virtual void onPause() noexcept {}
	virtual void onResume() noexcept {}
private:
	TStateType m_type;
};