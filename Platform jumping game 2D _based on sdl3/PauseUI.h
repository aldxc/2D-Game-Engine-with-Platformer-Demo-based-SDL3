#pragma once
#include <array>
#include "core/UI.h"
#include "UIType.h"
#include "Botton.h"
#include "Config.h"

class Input;
class EventManager;
class Renderer;

class PauseUI : public UI<UIType> {
public:
	explicit PauseUI(Input& iM, EventManager& eM, Renderer& r) noexcept;
	~PauseUI() noexcept override = default;
	void handleInput() noexcept override final;
	void update(double dt) noexcept override final;
	void render() const noexcept override final;
private:
	Renderer& renderer_; // 渲染器引用，供UI内的渲染相关操作使用
	Input& inputManager_; // 输入管理器引用，供UI内的输入相关操作使用
	EventManager& eventManager_; // 事件管理器引用，供UI内的事件相关操作使用

	std::array<Botton, Config::PAUSE_BOTTONS_NUM> bottons_;
};