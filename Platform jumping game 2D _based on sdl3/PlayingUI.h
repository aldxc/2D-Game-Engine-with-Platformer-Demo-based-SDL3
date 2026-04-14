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
	explicit PlayingUI(Input& iM, EventManager& eM, Renderer& r);
	~PlayingUI() override;
	void handleInput() noexcept override final;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
private:
	Renderer& renderer_; // 渲染器引用，供UI内的渲染相关操作使用
	EventManager& eventManager_; // 事件管理器引用，供UI内的事件相关操作使用
	Input& inputManager_; // 输入管理器引用，供UI内的输入相关操作使用
	//test
	std::array<Botton, 3> bottons_; // 测试按钮
};