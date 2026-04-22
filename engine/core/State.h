#pragma once

template<class TStateType>
class State {
public:
	explicit State(TStateType init) noexcept : type_(init) {}
	virtual ~State() = default;

	//禁用拷贝
	State(const State&) = delete;
	State& operator=(const State&) = delete;
	State(State&&) = delete;
	State& operator=(State&&) = delete;

	virtual void update(double dt) noexcept = 0;
	virtual void render() const noexcept = 0;
	bool isType(TStateType type) const noexcept { return type_ == type; } // 比较接口
	TStateType getType() const noexcept { return type_; } // 获取状态类型接口

	// 钩子函数，允许在状态切换时执行特定操作
	virtual void onEnter() noexcept {}
	virtual void onExit() noexcept {}
	virtual void onPause() noexcept {}
	virtual void onResume() noexcept {}
private:
	TStateType type_;
};