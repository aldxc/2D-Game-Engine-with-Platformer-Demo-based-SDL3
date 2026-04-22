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
	// ÒÀÀµ×é¼þ
	Renderer& renderer_; 
	Input& inputManager_; 
	EventManager& eventManager_; 

	std::array<Botton, Config::PAUSE_BOTTONS_NUM> bottons_;
};