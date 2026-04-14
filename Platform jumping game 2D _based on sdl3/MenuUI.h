#pragma once
#include <array>
#include "core/UI.h"
#include "UIType.h"
#include "Botton.h"
#include "Config.h"

class Renderer;
class EventManager;
class Input;

class MenuUI : public UI<UIType> {
public:
	explicit MenuUI(Input& iM, EventManager& eM, Renderer& r);
	~MenuUI() override;
	void handleInput() noexcept override final;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
private:
	Renderer& renderer_; // 渲染器引用，供UI内的渲染相关操作使用
	EventManager& eventManager_; // 事件管理器引用，供UI内的事件相关操作使用
	Input& inputManager_; // 输入管理器引用，供UI内的输入相关操作使用

	std::array<Botton, Config::MENU_BOTTOMS_NUM> bottons_; // start continue settings quit
};