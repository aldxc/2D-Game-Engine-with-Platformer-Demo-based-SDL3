#pragma once
#include <array>
#include "core/UI.h"
#include "Botton.h"
#include "Config.h"
#include "UIType.h"

class Renderer;
class EventManager;
class Input;

class LoseUI : public UI<UIType> {
public:
	explicit LoseUI(Input& inputManager, EventManager& eventManager, Renderer& r);
	~LoseUI() override;
	void handleInput() noexcept override final;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
private:
	Renderer& renderer_; // 渲染器引用，供UI内的渲染相关操作使用
	EventManager& eventManager_; // 事件管理器引用，供UI内的事件相关操作使用
	Input& inputManager_; // 输入管理器引用，供UI内的输入相关操作使用

	std::array<Botton, Config::WON_BUTTONS_NUM> bottons_; // lose  back to menu
};