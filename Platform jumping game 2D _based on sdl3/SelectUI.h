#pragma once
#include <array>
#include "UIType.h"
#include "core/UI.h"
#include "Config.h"
#include "Botton.h"

class Renderer;
class EventManager;
class Input;
class GameSession;

class SelectUI : public UI<UIType> {
public:
	explicit SelectUI(Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept;
	~SelectUI() noexcept override;
	void handleInput() noexcept override final;
	void update(double dt) noexcept override final;
	void render() const noexcept override final;
private:
	Renderer& renderer_; // 渲染器引用，供UI内的渲染相关操作使用
	EventManager& eventManager_; // 事件管理器引用，供UI内的事件相关操作使用
	Input& inputManager_; // 输入管理器引用，供UI内的输入相关操作使用
	GameSession& gameSession_;

	std::array<Botton, Config::SELECT_BUTTONS_NUM> bottons_; // pass the level next level  back to menu 
};