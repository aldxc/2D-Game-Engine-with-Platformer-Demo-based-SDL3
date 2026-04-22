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
	// ÒÀÀµ×é¼þ
	Renderer& renderer_;
	EventManager& eventManager_; 
	Input& inputManager_; 
	GameSession& gameSession_;

	std::array<Botton, Config::SELECT_BUTTONS_NUM> bottons_; // pass the level next level  back to menu 
};