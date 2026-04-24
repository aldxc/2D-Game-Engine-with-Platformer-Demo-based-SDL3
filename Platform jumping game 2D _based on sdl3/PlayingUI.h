#pragma once
#include <array>
#include "Botton.h"
#include "core/UI.h"
#include "UIType.h"

class Renderer;
class EventManager;
class Input;

class PlayingUI : public UI<UIType> {
public:
	explicit PlayingUI(Input& iM, EventManager& eM, Renderer& r) noexcept ;
	~PlayingUI() noexcept override;
	void handleInput() noexcept override final;
	void update(double dt) noexcept override final;
	void render() const noexcept override final;
private:
	Renderer& m_renderer; // 渲染器引用，供UI内的渲染相关操作使用
	EventManager& m_eventManager; // 事件管理器引用，供UI内的事件相关操作使用
	Input& m_inputManager; // 输入管理器引用，供UI内的输入相关操作使用
	//test
	std::array<Botton, Config::PLAYING_BUTTONS_NUM> m_bottons; // 测试按钮
};