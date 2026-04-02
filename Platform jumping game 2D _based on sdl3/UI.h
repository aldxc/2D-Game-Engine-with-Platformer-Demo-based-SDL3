#pragma once

enum class UIType { MENU, PLAYING, PAUSE, WON, LOST };

class UI {
public:
	UI(UIType type) : type_(type) {}
	virtual ~UI() = default;
	virtual void handleInput() noexcept = 0;
	virtual void update() noexcept = 0;
	virtual void render() const noexcept = 0;
	bool isType(UIType type) const noexcept { return type_ == type; } // 比较接口
private:
	UIType type_; //
};