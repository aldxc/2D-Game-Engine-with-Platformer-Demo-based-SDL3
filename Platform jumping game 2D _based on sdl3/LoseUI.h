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
	explicit LoseUI(Input& inputManager, EventManager& eventManager, Renderer& r) noexcept;
	~LoseUI() noexcept override;
	void handleInput() noexcept override final;
	void update(double dt) noexcept override final;
	void render() const noexcept override final;
private:
	// ÒÀÀµ×é¼þ
	Renderer& m_renderer;
	EventManager& m_eventManager;
	Input& m_inputManager; 

	std::array<Botton, Config::LOSE_BUTTONS_NUM> m_bottons; // lose  back to menu
};