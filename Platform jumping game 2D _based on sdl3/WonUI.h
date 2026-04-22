#pragma once
#include <array>
#include "core/UI.h"
#include "Botton.h"
#include "UIType.h"
#include "Config.h"

class Renderer;
class EventManager;
class Input;

class WonUI : public UI<UIType> {
public:
	explicit WonUI(Input& iM, EventManager& eM, Renderer& r) noexcept;
	~WonUI() noexcept override;
	void handleInput() noexcept override final;
	void update(double dt) noexcept override final;
	void render() const noexcept override final;
private:
	// ÒÀÀµ×é¼þ
	Renderer& renderer_;
	EventManager& eventManager_;
	Input& inputManager_;

	std::array<Botton, Config::WON_BUTTONS_NUM> bottons_; // pass the level next level  back to menu 
};