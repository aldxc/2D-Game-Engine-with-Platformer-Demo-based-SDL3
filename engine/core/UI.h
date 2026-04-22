#pragma once

template<class TUIType>
class UI {
public:
	explicit UI(TUIType type) noexcept : type_(type) {}
	virtual ~UI() = default;

	UI(const UI&) = delete;
	UI& operator=(const UI&) = delete;
	UI(UI&&) = delete;
	UI& operator=(UI&&) = delete;

	virtual void handleInput() noexcept = 0;
	virtual void update(double dt) noexcept = 0;
	virtual void render() const noexcept = 0;

	bool isType(TUIType type) const noexcept { return type_ == type; }
	TUIType getType() const noexcept { return type_; }

private:
	TUIType type_;
};